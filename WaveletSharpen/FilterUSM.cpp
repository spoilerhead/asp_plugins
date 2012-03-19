/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
 
 
//#define INSTRUMENTATION

 
#include "FilterUSM.h"
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

#define FilterName USMFilter

#define kernelsize 50.f //seems to work well, don't change unless nencessary, maybe need rebalance in additionalradius
//#define SUBSAMPLE
#define PADLIMIT  //don't process padding


bool FilterName::needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	bool okay;
	bool valUsm = layerOptions.getBool(EnableUSM, m_groupId, okay);

	bool retVal = valUsm ;
	#ifdef INSTRUMENTATION
	qDebug()<<name()<<"run? "<<valUsm<<" Return value:"<<retVal;
	#endif
	
	return retVal;
}

bool FilterName::isSlow(const ImageSettings &options, const PipeSettings  &settings) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	return false;// we're not that slow anymore ;) true;
}



bool FilterName::needsOriginalImage  ()
{
	return true; //we only use the current tile
}



int FilterName::additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const
{
    bool okay;
  	bool  enableUsm = layerOptions->getBool(EnableUSM, m_groupId, okay);
    float radiusUsm = layerOptions->getInt(RadiusUSM, m_groupId, okay)*zoomLevel;

	float effRadius = (enableUsm?(min(radiusUsm,2.f*kernelsize)*3.0f):0.0f);
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
    
    //const int totalPixels = width*height;
    //Make a copy of the original L channel
    pixel *org = new pixel[width*height];
    memcpy(org,fimg[0],width*height*sizeof(pixel));
    
    
    //------
    
    
    bool okay;
    // USM
    //bool  enableUSM = layerOptions.getBool(EnableUSM, m_groupId, okay);
    float amountUSM = layerOptions.getInt(AmountUSM, m_groupId, okay)/100.0f;//= 1.5; //0..10
    float radiusUSM = layerOptions.getInt(RadiusUSM, m_groupId, okay)*zoomLevel;///10.0f; //= 0.5; //0..2
    float thresholdUSM = layerOptions.getInt(ThresholdUSM, m_groupId, okay)/600.0f ;
    bool  clarityModeUSM = layerOptions.getBool(USMasClarity, m_groupId, okay);
      	                

    //---------------------------------------------------------------------------------------------

    #ifdef INSTRUMENTATION
    qDebug()<<name()<<"process: U: "<<" ru: "<<radiusUSM/zoomLevel<<" a: "<<amountUSM;
	#endif
    
/*    int addrad =additionalRadius(&layerOptions,zoomLevel);
    for(int x=0;x<=255;x++)
    for(int y=0;y<=255;y++) {
        int s = (x+2*addrad)*width+y+2*addrad;
        int d = (x+addrad)*width+y+addrad;
        fimg[0][d] = fimg[0][s];
        
    }*/
    
    
   //if(0) 
    {
        //Process
       	#ifdef INSTRUMENTATION
        qDebug()<<name()<<"USM RUNNING"<<" ru: "<<radiusUSM<<" w: "<<width<<" h: "<<height;
        #endif

    
        //USM_IIR(fimg[0],width,height,radiusUSM);

        #ifndef SUBSAMPLE
        USM_IIR_stacked(fimg[0],width,height,radiusUSM,kernelsize);
        #else
        int scale = 1;
        if(radiusUSM > 2.f) scale = 2.f;
        if(radiusUSM > 4.f) scale = 4.f;
        if(radiusUSM > 8.f) scale = 8.f;
        if(radiusUSM >16.f) scale =16.f;
        if(radiusUSM >32.f) scale =32.f;
        #error: subsampling needs some love so all tiles pick the same smaples. mips?

        int ws = width/scale;
        int hs = height/scale;
        pixel *small = new pixel[ws*hs];
        
        //for(int i=0;i<(hs*ws);i++) {
        for(int x = 0; x<ws;x++)
        for(int y = 0; y<hs;y++) {
            small[x*ws+y] = fimg[0][x*scale*width+y*scale];    //subsample
        }
        USM_IIR_stacked(small,ws,hs,radiusUSM/scale,kernelsize);
        #endif
        
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
          
          #ifndef SUBSAMPLE
          pixel diff = (org[i] - fimg[0][i]);                                         //org-usm = highpass
          #else
          //pixel diff = (org[i] - small[(x/scale)*ws+(y/scale)]);                                         //org-usm = highpass
          pixel diff = (org[i]-bLinInt(small,ws,hs,x,y, scale));
          #endif
          diff = (diff>0.0f)?max(diff-thresholdUSM,0.0f):min(diff+thresholdUSM,0.0f); //threshold
          diff *= amountUSM;
          
          //emulate clarity, apply usm based on luminance, most on midtonse, none on black and white
          if(clarityModeUSM) {
              float clarity =  (
                                    1.f-fabs(2.f*(org[i]-0.5f))      //1 at mid gray, 0 at 0 and 1
                               );
              diff *= clarity;
          }
          
          //---
         fimg[0][i] = clip(org[i] + diff,0.0f,1.0f); //USM step

        }
        #ifndef SUBSAMPLE
        #else
        delete[] small;
        #endif
    }
    
   
    //---------------------------------------------------------------------------------------------        
    delete[] org; //Free memory
    
}

