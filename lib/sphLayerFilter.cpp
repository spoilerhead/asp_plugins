/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010-2012
 * licensed: GPL v2+, other licenses  available on request
 */

#include "sphLayerFilter.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"
#include <QDebug>

#include <iostream>
#include <cmath>


#include "sphTileCache.h"
#include "sqrtlut.h"
//Hacked up by Spoilerhead

#define FilterName sphLayerFilter
//#define INSTRUMENTATION


//===========================================================================================================================
// don't touch here until sure
//===========================================================================================================================

bool FilterName::isLargeRadius(const ImageSettings &options, const PipeSettings  &settings) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	return true; //we only use the current tile
}

//compute the buffer rectangle around the tile. may contain non existing coorinates (outside of image)
QRect FilterName::getBufferRect(PluginOptionList  *layerOptions, const PipeSettings  &settings, PluginTile &tile) const
{
    QRect tileRect = QRect(QPoint(tile.x(),tile.y()), settings.tileSize()); 
    //use really needed radius
    const int pad = settings.tilePad();
    float zoomLevel = pow(0.5f,settings.mip());
    
	const int tileExtension = max(additionalRadius(layerOptions,zoomLevel), pad);//allways process padding
    QRect srcRect     = tileRect.adjusted(-tileExtension, -tileExtension, tileExtension, tileExtension); //expand by neighborTiles Tiles in each direction
    return srcRect;
}

QRect FilterName::getPrerequisiteRect(PluginOptionList  *layerOptions, const PipeSettings  &settings, PluginTile &tile) const
{
    const int srcStep = m_hub->step(this) - 1;
	QRect srcRect = getBufferRect(layerOptions,settings,tile);  //get the buffer rectangle
    QRect nRect       = srcRect.intersected(QRect(QPoint(0,0),  settings.size(srcStep))); //intersect srcRect with the image itself, so we don't request non existing tiles
    
    
/*    qDebug()<<"------------------------------------------------------";
    qDebug()<<"T : "<<tileRect.left()<<"/"<<tileRect.top()<<"   /   "<<tileRect.right()<<"/"<<tileRect.bottom();
    qDebug()<<"TE: "<<tileExtension<<" nT: "<<neighborTiles;
    qDebug()<<"TS: "<<settings.tileSize().width()<<" / "<<settings.tileSize().height();
    qDebug()<<"IM: "<<settings.size(srcStep).width()<<" / "<<settings.size(srcStep).height();
    qDebug()<<"S : "<<srcRect.left()<<"/"<<srcRect.top()<<"   /   "<<srcRect.right()<<"/"<<srcRect.bottom();
    qDebug()<<"N : "<<"Step: "<<srcStep<<"  mip: "<<settings.mip()<<" "<<nRect.left()<<"/"<<nRect.top()<<"   /   "<<nRect.right()<<"/"<<nRect.bottom();
    qDebug()<<"------------------------------------------------------";  
  */  
    
    if(nRect.width()<=0 || nRect.height()<=0) {
        qDebug()<<"WARNING"<<name()<<": Invalid tile ROI computed for Tile ("<<tile.x()  <<","<<tile.y() <<") - image size:"<<settings.size(srcStep);
        return QRect(0,0,0,0);
    }
    return nRect;  
}

QList<PluginDependency*> FilterName::prerequisites(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile) const
{ 
    Q_UNUSED(options);
	int srcStep = m_hub->step(this) - 1;
    QRect nRect;
    for(int layer=0;layer<options.count();layer++) {    //get rect for each layer
       nRect  |= getPrerequisiteRect(options.options(layer), settings,tile); //union
    }
    //qDebug()<<"Prereq: top: "<<nRect.x()<<","<<nRect.y()<<"  - "<<nRect.width()<<","<<nRect.height();
    if(nRect.width()<=0 || nRect.height()<=0) {
        return QList<PluginDependency*>();	//	Empty list - we don't depend on any other tiles
    }
    return settings.tilesIn(nRect, srcStep,settings.mip());
}


void FilterName::runLayer(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const
{
    if(options.options(layerPos)==NULL) return; // FIX: ----------- otherwise crashes on Print(?)
    
    const bool runsOnLayer = (layerPos != 0);
    #ifdef INSTRUMENTATION
    qDebug()<<"INFO"<<name()<<": LayerPos: "<<layerPos<<"( Mip: "<<settings.mip()<<"  Coords:"<<tile.x()  <<","<<tile.y()<<")";
    #endif
    
    PluginImage *targetImage;
    
    if(runsOnLayer) targetImage = tile.tmpImage();
    else            targetImage = tile.image();
        
    const int srcStep = m_hub->step(this) - 1;
   
    PluginImage *image = tile.image();
	const int pw = image->planeWords();		// increment to get to the next pixel
    const int np = image->nextPixel();
    const int pad = settings.tilePad();
    //const int padh= pad;                      //Paddings
    //const int padw=pad;
    
    
    //Read size extension ------------------------------------------------------------------------
	const float zoomLevel = pow(0.5f,settings.mip());
//	const float radius = max(additionalRadius(&layerOptions,zoomLevel), pad);//allways process padding
//	const float iradus = int(radius);
    //---------------------------------------------------------------------------------------------
    
    QRect srcRect  = getPrerequisiteRect(&layerOptions,settings,tile);                    //Pixels we requested
    QRect tileRect = QRect(QPoint(tile.x(),tile.y()), settings.tileSize()); //current tile
    QRect buffRect = getBufferRect(&layerOptions,settings,tile); //buff may be lager than src rect (negative pixels get mirrored)

    if( (srcRect.width() <= 0) || (srcRect.height() <=0) ) {    //invalid ROI
        //qDebug()<<"buffer: (" <<bufferLeft<<","<<bufferTop<<") "<<bufferWidth<<"x"<<bufferHeight<<"\t src: ("<<srcRect.left()  <<","<<srcRect.top() <<") "<<srcRect.width() <<"x"<<srcRect.height();
        //srcRect = tileRect;
        //buffRect = tileRect;
        qDebug()<<"INFO"<<name()<<": Invalid ROI for tile: ("<<tileRect.left()  <<","<<tileRect.top() <<") "<<tileRect.width() <<"x"<<tileRect.height()<<" - skipping";
        return;
        
    }
    
    const int bufferWidth  = buffRect.width();
	const int bufferHeight = buffRect.height();
	const int bufferLeft   = buffRect.left();
	const int bufferTop    = buffRect.top();
	
	const int totalPixels = (bufferWidth*bufferHeight);
	
    #define NUMCHAN 3
	pixel *fimg[NUMCHAN];                                                   //Allocate Buffers
    for (int i = 0; i < NUMCHAN; i++) {
         fimg[i] = new pixel[totalPixels];
    }

   
    
// ======== Read in tiles into fimg ===========================================================
    int px_i = 0;

    sphPixelCache pixels(srcRect,settings,srcStep);     //Initialize PixelCache
    try {
        for(int y = 0; y < bufferHeight; y++ ) { 
        for(int x = 0; x < bufferWidth ; x++ ) { 
            //world coordinates
            int wx = bufferLeft + x;
            int wy = bufferTop  + y;
            
            uint16 *pIn = pixels.getPixel(wx,wy);   //Get address of Pixel
            
            
            uint16 iR,iG,iB;
            if(pIn != NULL) {
            	iR = *(pIn);
		        iG = *(pIn +  pw);
		        iB = *(pIn + 2*pw);
		    } else { // give it a pretty average init :D
      		    iR = 0x7fff;
		        iG = 0x7fff;
		        iB = 0x7fff;
		    }
		    
		    float r,g,b;            //use lut based conversion and deGamma
		    switch(m_deGamma) {
		        case GAMMA_ALL:     r = sqrtLUT[iR];
		                            g = sqrtLUT[iG];
		                            b = sqrtLUT[iB];
		                            break;
		        case GAMMA_FIRST:   r = sqrtLUT[iR];
                    		        g = to_workspace(iG);
		                            b = to_workspace(iB);
		                            break;
		        default:            r = to_workspace(iR);
		                            g = to_workspace(iG);
		                            b = to_workspace(iB);
		                            break;
		    
		    }
		    
		    
		    //Color space conversion
		    switch(m_conversion) {
		        case RGB2YCBCR:
		            rgb2ycbcrf(&r,&g,&b);
		            break;
		        case RGB2HSL:
		            rgb2hsl(&r,&g,&b);
		            break;
		        case RGB2HSV:
		            rgb2hsv(&r,&g,&b);
		            break;
                case RGB2HCL:
		            rgb2hcl(&r,&g,&b);
		            break; 
		        case RGB2LAB:
		            rgb2lab(&r,&g,&b);
		            break;
		        case RGB2XYZ:
		            rgb2lab(&r,&g,&b);
		            break; 
		        case NONE:
		        default:
		            break;
		    }
		    
		
		
    		fimg[0][px_i] = r;
	        fimg[1][px_i] = g;
	        fimg[2][px_i] = b;
            // ===== Final step, inc pointer =====
		    px_i++;
	    }
	    }
	} catch(...) {  //catch exceptions from bibble and release memory
	    for (int i = 0; i < NUMCHAN; i++) {
            delete[] fimg[i];
        }
	    throw;
	}
	
	// ==== PROCESS HERE ==============================================================================
	#ifdef INSTRUMENTATION
    qDebug()<<"(info) procWidth: "<<bufferWidth<<" procHeight: "<<bufferHeight<<"( Mip: "<<settings.mip()<<"  Coords:"<<tile.x()  <<","<<tile.y()<<")";
    #endif
	
	if((bufferWidth >0) && (bufferHeight>0)) ProcessBuffer(fimg,bufferWidth,bufferHeight,zoomLevel,options,settings,tile,layerOptions,layerPos);
	else qDebug()<<"negative sized tile! "<<bufferWidth<<"x"<<bufferHeight;
	//=================================================================================================
	#ifdef INSTRUMENTATION
    qDebug()<<"(info) processed ( Mip: "<<settings.mip()<<"  Coords:"<<tile.x()  <<","<<tile.y()<<")";
    #endif
	
	// =============== WRITE DATA to processed tile ============
	px_i = 0;
    uint16 *pOut = targetImage->data16();
    for(int y = -pad; y < tileRect.height() + pad; y++ ) {
        for(int x = -pad; x < tileRect.width() + pad; x++ ) {
            //world coordinates
            int wx = tileRect.left() + x;
            int wy = tileRect.top()  + y;
            
            //buffer coordinates
            int bx = wx - bufferLeft;
            int by = wy - bufferTop;

            px_i = by*bufferWidth+bx;    
            
            float r = fimg[0][px_i];
            float g = fimg[1][px_i];
            float b = fimg[2][px_i];
            
            //Color space conversion
		    switch(m_conversion) {
		        case RGB2YCBCR:
		            ycbcr2rgbf(&r,&g,&b);
		            break;
		        case RGB2HSL:
		            hsl2rgb(&r,&g,&b);
		            break;
		         case RGB2HSV:
		            hsv2rgb(&r,&g,&b);
		            break;
		        case RGB2HCL:
		            hcl2rgb(&r,&g,&b);
		            break;
		        case RGB2LAB:
		            lab2rgb(&r,&g,&b);
		            break;
		        case RGB2XYZ:
		            xyz2rgb(&r,&g,&b);
		            break; 
		        case NONE:
		        default:
		            break;
		    }
		    
		    switch(m_deGamma) {
		        case GAMMA_ALL:     b *= b;
		                            g *= g;
		        case GAMMA_FIRST:   r *= r;
		        default:            break;
		    }

            *(pOut)         = from_workspace(r); //R
            *(pOut + pw)    = from_workspace(g); //G
            *(pOut + 2*pw)  = from_workspace(b); //B
            pOut += np;
	    }
	}	
	
	for (int i = 0; i < NUMCHAN; i++) {
         delete[] fimg[i];
    }
}

