#include "FilterAngy.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"

#include "colorspace.h"
#include "blend_modes.h"
#include "fastmath.h"

#include <QDebug>
#include <iostream>
#include <cmath>

#include "sqrtlut.h"


#define FILTERNAME AngyFilter

#define FASTLOCAL static inline
//#define FASTLOCAL 

using namespace std;

//================================================================================

bool FILTERNAME::needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList &layerOptions) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	bool okay;
	bool val = layerOptions.getBool(FILTERNAME::Enable, m_groupId, okay);
	return val;
}

bool FILTERNAME::isSlow(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;
}

bool FILTERNAME::isLargeRadius(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;	//	We only use the current tile.
}

bool FILTERNAME::isPixelSource(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;	//	We don't generate pixels from scratch.
}

QTransform FILTERNAME::transform(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return QTransform();	//	We don't warp the image so return an identity transform.
}

QSize FILTERNAME::size(const ImageSettings &options, const PipeSettings  &settings, const QSize &startSize) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return startSize;	//	We don't change the size of the image.
}

QList<PluginDependency*> FILTERNAME::prerequisites(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	Q_UNUSED( tile );
	
	return QList<PluginDependency*>();	//	Empty list - we don't depend on any other tiles.
}




void FILTERNAME::runLayer(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList &layerOptions, int layerPos) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	//Q_UNUSED( layerOptions );
	
	PluginImage *	pSrcImg = tile.image();
	PluginImage *	pDstImg;
	
	//	Layer 0 output should always go into tile.image(), otherwise output should go into tile.tmpImage()
	if ( layerPos == 0 )
		pDstImg = tile.image();
	else
		pDstImg = tile.tmpImage();
		
	if((pSrcImg == NULL) || (pDstImg == NULL)) return; //NULLPointer check for safety
	
	int rw = pSrcImg->rowWords();	//	number of unsigned shorts in a row 
	int pw = pSrcImg->planeWords();	//	increment to get to the next color component in the plane
	int np = pSrcImg->nextPixel();	//	increment for the next pixel within the same color plane - if the pointer is
									//	pointing at a green pixel this incremet will point to the next green pixel

	//	Use paddedWidth & paddedHeight to get the padded width & height of the tile.
	//	We could also use image->paddedWidth() & image->paddedHeight().
	int tileWidth = settings.paddedWidth();        //Tile Dimensions
	int tileHeight = settings.paddedHeight();

	uint16 * pSrc = NULL;
	uint16 * pOut = NULL;
	
	uint16 iR,iG,iB;
	bool okay;
	
	
	// ==== Read Options ======================================================
	
	bool  optBleach     = layerOptions.getBool(Bleach, m_groupId, okay); 
	float optScreen     = layerOptions.getInt(Screen, m_groupId, okay)/100.f;
	float optTotal      = layerOptions.getInt(Total, m_groupId, okay)/100.f;
	float optFlatten    = layerOptions.getInt(Flatten, m_groupId, okay)/100.f;
	float optBrightness = 0.5f+(layerOptions.getInt(Brightness, m_groupId, okay)/100.f);	
	float optRecovery   = layerOptions.getInt(Recovery, m_groupId, okay)/50.f;
	float optDegamma    = layerOptions.getInt(Degamma, m_groupId, okay)/100.f;
	
	
	
	
    rgb_color rgb;
	
    for ( int y = 0; y < tileHeight; y++ ) {
		//	image-data16() points to the very first unpadded pixel
		//	add to that (rw * y) to point us at the first unpadded pixel for line 'y'
		pSrc = pSrcImg->data16() + rw * y;
		pOut = pDstImg->data16() + rw * y;

		// ======== Per Pixel Loop ============================================
		for ( int x = 0; x < tileWidth ; x++, pOut += np, pSrc += np ) { //	move to the next pixel (in the color plane)
		    // == read from image plane
			iR = *(pSrc)        ;
			iG = *(pSrc + pw)   ;
			iB = *(pSrc + 2*pw) ;
            
            //lut based int to float and gamma
            rgb.r = sqrtLUT[iR];
            rgb.g = sqrtLUT[iG];
            rgb.b = sqrtLUT[iB];
            //------------------------------------
            
            rgb2ycbcrf(&rgb.r,&rgb.g,&rgb.b);
            
            if(optDegamma != 1.f) rgb.r = DEGAMMA(rgb.r,optDegamma); 
                           
            float brightness = clipf(rgb.r * optBrightness,0.0f,1.0f);
            rgb.r = SHADOW_RECOVERY(rgb.r, optRecovery);
            
            
            //sharpening step ommited
            
            //Flatten
            brightness = SHADOW_RECOVERY(brightness,2.f*optFlatten);
            
            //screen layers, parametrized opacity
            float screen = BLEND( SCREEN(brightness,brightness),brightness, optScreen);
            if(optBleach) {
                rgb.r = BLEND( OVERLAY(rgb.r,screen),rgb.r, optTotal);
            } else {
                rgb.r = BLEND( OVERLAY(screen,rgb.r),rgb.r, optTotal);
            }
            rgb.r = clipf(rgb.r,0.f,1.f);
         
            if(optDegamma != 1.f) rgb.r = GAMMA(rgb.r,optDegamma); 
         
         
            ycbcr2rgbf(&rgb.r,&rgb.g,&rgb.b);
            //------------------------------------
			//Back to float and degamma
    		iR = FTOI16(rgb.r*rgb.r);
			iG = FTOI16(rgb.g*rgb.g);
			iB = FTOI16(rgb.b*rgb.b);
			
			//iG = iB = iR;
			
			// == Write back to image plane
			*(pOut)        = iR;
			*(pOut + pw)   = iG;
			*(pOut + 2*pw) = iB;
		} // ======== End Pixel Loop ==========================================
	}
}
