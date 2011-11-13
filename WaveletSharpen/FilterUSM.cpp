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

 	
	float effRadius = (enableUsm?(radiusUsm*3.0f):0.0f);
	
    return effRadius;
}


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
    
    //if(enableUSM) 
    {
        //Process
       	#ifdef INSTRUMENTATION
        qDebug()<<name()<<"USM RUNNING"<<" ru: "<<radiusUSM<<" w: "<<width<<" h: "<<height;
        #endif


        USM_IIR(fimg[0],width,height,radiusUSM);

        //Blend
        for(int i = 0;i<(width*height);i++) {
          pixel diff = (org[i] - fimg[0][i]);                                         //org-usm = highpass
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
    }
    
   
    //---------------------------------------------------------------------------------------------        
    delete[] org; //Free memory
}

