/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2012
 * licensed: GPL v2+
 */
 
 
//#define INSTRUMENTATION

 
#include "FilterPyramid.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"
#include <QDebug>


#include <iostream>

#include "usm_iir.h"
//#include "gauss_iir.h"

using namespace std;

#define FilterName PyramidFilter
#define MAXLAYERS (8)
#define CHANNELS (1)        //change when rgb

#define kernelsize 50.f //seems to work well, don't change unless nencessary, maybe need rebalance in additionalradius
//#define SUBSAMPLE
#define PADLIMIT  //don't process padding


bool FilterName::needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	bool okay;
	bool val = layerOptions.getBool(EnablePyramid, m_groupId, okay);

	bool retVal = val ;
	#ifdef INSTRUMENTATION
	qDebug()<<name()<<"run? "<<val<<" Return value:"<<retVal;
	#endif
	
	return retVal;
}

bool FilterName::isSlow(const ImageSettings &options, const PipeSettings  &settings) const {
    Q_UNUSED(options);
    Q_UNUSED(settings);
	return false;// we're not that slow anymore ;) true;
}



bool FilterName::needsOriginalImage  () {
	return true; //we only use the current tile
}

int FilterName::getLastNonZeroPyramid(PluginOptionList  *layerOptions) const {
    bool okay;
    if( layerOptions->getInt(P8, m_groupId, okay) != 0 ) return 7;
    if( layerOptions->getInt(P7, m_groupId, okay) != 0 ) return 7;
    if( layerOptions->getInt(P6, m_groupId, okay) != 0 ) return 6;
    if( layerOptions->getInt(P5, m_groupId, okay) != 0 ) return 5;
    if( layerOptions->getInt(P4, m_groupId, okay) != 0 ) return 4;
    if( layerOptions->getInt(P3, m_groupId, okay) != 0 ) return 3;
    if( layerOptions->getInt(P2, m_groupId, okay) != 0 ) return 2;
    if( layerOptions->getInt(P1, m_groupId, okay) != 0 ) return 1;
    return 0;       //all inactive
    
}

int FilterName::additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const {
    bool okay;
  	bool  enable = layerOptions->getBool(EnablePyramid, m_groupId, okay);
    float radius = pow(2.f,(float)getLastNonZeroPyramid(layerOptions))*zoomLevel;//layerOptions->getInt(RadiusUSM, m_groupId, okay)*zoomLevel;

	float effRadius = (enable?(min(radius,2.f*kernelsize)*3.0f):0.0f);
    return effRadius;
}

#ifdef SUBSAMPLE

static inline float LERP(
   const float y1,const float y2,
   const float mu)
{
   return(y1*(1.f-mu)+y2*mu);
}

static inline pixel bLinInt(pixel *small, const int ws, const int hs, const int x, const int y, const int scale) {
    int xs = (x/scale);
    int ys = (y/scale);
    float xmod = (x-(xs*scale))*(1.f/scale);
    float ymod = (y-(ys*scale))*(1.f/scale);
    
    
    pixel intlo =  LERP(small[ xs   *ws + ys], small[ xs   *ws + ys+1], ymod );
    pixel inthi =  LERP(small[(xs+1)*ws + ys], small[(xs+1)*ws + ys+1], ymod );
    

    return LERP(intlo,inthi,xmod);
}
#endif

void FilterName::ProcessBuffer(pixel *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
    Q_UNUSED(tile);
    Q_UNUSED(layerPos);
    
   
    //------
    
    
    bool okay;
    // USM
    //bool  enableUSM = layerOptions.getBool(EnableUSM, m_groupId, okay);
    float coeff[MAXLAYERS];
    coeff[0] = layerOptions.getInt(P1, m_groupId, okay)/100.0f+1.f;
    coeff[1] = layerOptions.getInt(P2, m_groupId, okay)/100.0f+1.f;
    coeff[2] = layerOptions.getInt(P3, m_groupId, okay)/100.0f+1.f;
    coeff[3] = layerOptions.getInt(P4, m_groupId, okay)/100.0f+1.f;
    coeff[4] = layerOptions.getInt(P5, m_groupId, okay)/100.0f+1.f;
    coeff[5] = layerOptions.getInt(P6, m_groupId, okay)/100.0f+1.f;
    coeff[6] = layerOptions.getInt(P7, m_groupId, okay)/100.0f+1.f;
    coeff[7] = layerOptions.getInt(P8, m_groupId, okay)/100.0f+1.f;
    //coeff[8] = layerOptions.getInt(P9, m_groupId, okay)/100.0f+1.f;
    
    //generate parabolic parameters
	float shadow = layerOptions.getInt(Shadow, m_groupId, okay)/100.0f;;
	float mid    = layerOptions.getInt(Mid, m_groupId, okay)/100.0f;;
	float lights = layerOptions.getInt(Light, m_groupId, okay)/100.0f;;
	const float denom = -0.25f;//(0.0f - 0.5f) * (0.0f - 1.0f) * (0.5f - 1.0f);
    float A     = (1.0f * (mid - shadow) + 0.5f * (shadow - lights) + 0.0f * (lights - mid)) / denom;
    float B     = (1.0f*1.0f * (shadow - mid) + 0.5f*0.5f * (lights - shadow) + 0.0f*0.0f * (mid - lights)) / denom;
    float C     = (0.5f * 1.0f * (0.5f - 1.0f) * shadow + 1.0f * 0.0f * (1.0f - 0.0f) * mid + 0.0f * 0.5f * (0.0f - 0.5f) * lights) / denom;
    
    const float radius[MAXLAYERS] = {0.f,1.f,1.f,2.f,4.f,8.f,16.f,32.f/*,64.f*/};
    //---------------------------------------------------------------------------------------------
    #ifdef INSTRUMENTATION
	qDebug()<<name()<<"runing "<<name()<<" coeffs =["<<coeff[0]<<", "<<coeff[1]<<", "<<coeff[2]<<", "<<coeff[3]<<", "<<coeff[4]<<", "<<coeff[5]<<", "<<coeff[6]<<", "<<coeff[7]/*<<", "<<coeff[8]*/<<"]";
	#endif

    const int ActiveLayers = getLastNonZeroPyramid(&layerOptions)+1;

    pixel *layers[MAXLAYERS];
    for(int i = 0;i<ActiveLayers;i++) layers[i] =  new pixel[width*height];
    
    for(int channel = 0;channel < CHANNELS;channel++) {
        memcpy(layers[0],fimg[channel],width*height*sizeof(pixel));

        //Compute Blur Pyramids
        for(int i = 0;i<(ActiveLayers-1);i++) {
            memcpy(layers[i+1],layers[i],width*height*sizeof(pixel));   //copy lower pyramid level
            USM_IIR_stacked(layers[i+1],width,height,radius[i+1]*zoomLevel,kernelsize);
        }
     
       
        
        #ifdef PADLIMIT
        const int pad = settings.tilePad();
        int addRad = additionalRadius(&layerOptions,zoomLevel);
        addRad = max(addRad-pad,0);
        //Blend
        
        //for(int x = 0; x<width;x++)
        //for(int y = 0; y<height;y++) {
        for(int x = addRad; x<(width-addRad);x++)
        for(int y = addRad; y<(height-addRad);y++) {
          const int i = x*width+y;
        #else
        for(int i = 0;i<(width*height);i++) {
        #endif  
            for(int lay = 0;lay<(ActiveLayers-1);lay++) {
                layers[lay][i] -= layers[lay+1][i];    //Create layer differences
            }
          
          
            pixel sum = 0.f;
            for(int lay = 0;lay<(ActiveLayers-1);lay++) {
                sum += layers[lay][i]*(coeff[lay]);
            }
            sum += ((layers[ActiveLayers-1][i]-0.5f)*(coeff[ActiveLayers-1]))+0.5f;       //residual isn't +/- 0
            
            pixel org = fimg[channel][i];       //original image
            pixel luma = layers[ActiveLayers-1][i]+layers[ActiveLayers-2][i];   //use residual
            float blendlevel = clip((A*(luma*luma)+B*luma+C),0.0f,1.0f);  //calculate alpha of the original image, based on the luminance
            
            fimg[channel][i] = clip(BLEND(sum,org,blendlevel),0.0f,1.0f); //USM step
        }


        
       
        //---------------------------------------------------------------------------------------------        
        
    }
    for(int i = 0;i<ActiveLayers;i++) delete[] layers[i]; //Free memory
    
}

