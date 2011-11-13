/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
 
 
//#define INSTRUMENTATION

#define BLEND_LAST_STEP //if defined Blend operation will use the last stage instead of the original input
//use usm for edge detection, otherwise use residuals from waveltsharpening
//the residuals result in a bit more haloing but are faster
#define USE_USM_EDGEDETECTION
 
 
#include "FilterLLorens.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"
#include <QDebug>


#include <iostream>
//Hacked up by Spoilerhead

#include "wavelet.h"
#include "dePepper.h"
#include "LlorensSharpen.h"

#include "usm_iir.h"
#include "gauss_iir.h"


#define wavelet_sharpen wavelet_sharpenF 
#define dePepper dePepperF
using namespace std;

#define FilterName LLorensFilter






bool FilterName::needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	bool okay;

    bool valLL  = layerOptions.getBool(EnableLL, m_groupId, okay);
	
	
	bool retVal = valLL;
	#ifdef INSTRUMENTATION
	qDebug()<<name()<<"run? LL:"<<valLL<<" Return value:"<<retVal;
	#endif
	
	return retVal;
}

bool FilterName::isSlow(const ImageSettings &options, const PipeSettings  &settings) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	return true;
}



bool FilterName::needsOriginalImage  ()
{
	return true; //we only use the current tile
}

int FilterName::additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const
{
    bool okay;
   /* bool  enable = layerOptions->getBool(Enable, m_groupId, okay);
    float radius = layerOptions->getDouble(Radius, m_groupId, okay)*zoomLevel;
    bool  enable2 = layerOptions->getBool(Enable2, m_groupId, okay);
	float radius2 = layerOptions->getDouble(Radius2, m_groupId, okay)*zoomLevel;
*/
    //TODO: additional radius for LLorens?
   	
	float effRadius = 1;//(enable?(radius+1.0f):1.0f)*(enable2?(radius2+1.0f):1.0f);
	
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
   
	    //LLorens Sharpening
	    bool enableLL  = layerOptions.getBool(EnableLL, m_groupId, okay);
	    int llStrength = layerOptions.getInt(LLStrength, m_groupId, okay)*zoomLevel;
	    int llIters    = layerOptions.getInt(LLIter, m_groupId, okay);
	    int mcStrength = layerOptions.getInt(MCStrength, m_groupId, okay)*zoomLevel;
            

        //---------------------------------------------------------------------------------------------
        #ifdef INSTRUMENTATION
	    qDebug()<<name()<<"process: LL: "<<enableLL;
    	#endif
        //Gratiend (LLorens) sharpening 
        //---------------------------------------------------------------------------------------------        
        if(enableLL) {
            if(llStrength>0 && llIters >0) LlorensSharpen(fimg[0], width,height,llIters,llStrength/100.0f);
            if(mcStrength >0) microcontrast(fimg[0], width,height,mcStrength/100.0f);
        }
        //---------------------------------------------------------------------------------------------        
        delete[] org;
        //Free memory

}

