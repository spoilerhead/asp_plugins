/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
#include "DenoiseFilter.h"
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
#include "convolution.h"
//#include "usm_iir.h"


//#define INSTRUMENTATION

//if defined Blend operation will use the last stage instead of the original input
#define BLEND_LAST_STEP

//#define wavelet_Denoise wavelet_DenoiseF 
#define dePepper dePepperF
using namespace std;

#define FilterName DenoiseFilter

float inline SettingToValue(const float value) {
    return (0.5f*exp(0.06f*value))-0.5f;
}

bool FilterName::needsToRunLayer(const ImageSettings &options, const PipeSettings &settings, PluginOptionList  &layerOptions) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	bool okay;
	bool val    = layerOptions.getBool(Enable, m_groupId, okay);
		
	bool retVal =  val;
	#ifdef INSTRUMENTATION
	qDebug()<<this->name()<<" run? 1:"<<val<<endl;
	#endif
	return retVal;
}

bool FilterName::isSlow(const ImageSettings &options, const PipeSettings &settings) const
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
    bool  enable = layerOptions->getBool(Enable, m_groupId, okay);
    
    float tres[2];
        tres[0] = /*SettingToValue*/(layerOptions->getDouble(TresL, m_groupId, okay))/3.0f;
	    tres[1] = /*SettingToValue*/(layerOptions->getDouble(TresC, m_groupId, okay))/3.0f; 
	
	float effRadius = (enable?(max(tres[0],tres[1])  ):0.0f);

    return effRadius;
}

static inline float hardT(const float value,const float t) { 
    //    return (value>=t)?0.f:1.f;
    return 1.f-min(value,0.95f);
}



//compute the edge mask of the image
static inline void edgeMaskB(float *fimg, float *fimgG,float *buf1,float *buf2,const unsigned int width, const unsigned int height) {

    //CHARR
    const float gradient[3] = {3.f/16.f,10.f/16.f,3.f/16.f};
    const float gradt[3] = {1.f,0.f,-1.f};
    
    float *gradX = buf1;
    float *gradY = buf2;

    //use convolution instead (Faster)
    const float gauss2px[3] = {.25f,0.5f,.25f};
    convolve2DSeparable(fimg,fimgG,width,height,gauss2px,3,gauss2px,3);

    convolve2DSeparable(fimgG,gradX,width,height,gradient,3,gradt,3);
    convolve2DSeparable(fimgG,gradY,width,height,gradt,3,gradient,3);
    const float gradTres = 0.050f;
    const float boost = 10.f;
        
    for(int i = 0; i<(width*height);i++) {  //create hard edges
        //qDebug()<<gradX[i]<<"  "<<fsgn(gradX[i]);
        fimgG[i] = hardT(boost*fastsqrt/*sqrtf*/(gradX[i]*gradX[i]+gradY[i]*gradY[i]), gradTres);
    }
    
}

static inline void edgeMask(float *fimg, float *fimgG,const unsigned int width, const unsigned int height) {
    float *gradX = new float[width*height];
    float *gradY = new float[width*height];

    edgeMaskB(fimg,fimgG,gradX,gradY,width,height);
    
    delete[] gradX;
    delete[] gradY;

}

void FilterName::ProcessBuffer(pixel *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
    Q_UNUSED(tile);
    Q_UNUSED(layerPos);
    
    const int totalPixels = width*height;

        
    #define NUMBUFFERS 3
    pixel *buffer[NUMBUFFERS];
    for (int i = 1; i < NUMBUFFERS; i++) {//don't alloc buffer 0
        buffer[i] = new pixel[width*height];
    }

    bool okay;
    bool  enable = layerOptions.getBool(Enable, m_groupId, okay);
    float low[3], tres[3];
    low[0]  = SettingToValue(layerOptions.getDouble(LowL, m_groupId, okay))/200.0f;
    low[1]  = SettingToValue(layerOptions.getDouble(LowC, m_groupId, okay))/200.0f;
    low[2]  = SettingToValue(layerOptions.getDouble(LowC, m_groupId, okay))/200.0f;
    tres[0] = SettingToValue(layerOptions.getDouble(TresL, m_groupId, okay))/10.0f*zoomLevel;
    tres[1] = SettingToValue(layerOptions.getDouble(TresC, m_groupId, okay))/10.0f*zoomLevel;
    tres[2] = SettingToValue(layerOptions.getDouble(TresC, m_groupId, okay))/10.0f*zoomLevel; 
    bool doDePepper = layerOptions.getBool(DePepper, m_groupId, okay);
    bool  showEdges = layerOptions.getBool(ShowEdges, m_groupId, okay);
	    

    //compute edge mask  
    float *fimgG = new float[totalPixels];
    //edgeMask(fimg[0], fimgG, width,  height);
    edgeMaskB(fimg[0], fimgG, buffer[1],buffer[2], width,  height);
    float *fimgOrg = new float[totalPixels];
    
    //qDebug()<<tres[0]<<" "<<tres[1]<<" "<<tres[2]<<" ";
    /*if(enable && !showEdges) {
        for(int i = 0;i<3;i++) {
            buffer[0] = fimg[i];
            memcpy(fimgOrg,fimg[i],totalPixels*sizeof(float));
            wavelet_denoise(buffer,width,height ,tres[i],low[i]);
            for(int j=0;j<width*height;j++) 
                fimg[i][j]= BLEND(fimg[i][j],fimgOrg[j],fimgG[j]);
            if(doDePepper) dePepper(buffer,width,height);
        }
    }*/
    if(enable) {
        if(showEdges) {
            memcpy(fimg[0],fimgG,totalPixels*sizeof(float));
        } else {
            memcpy(fimgOrg,fimg[0],totalPixels*sizeof(float));          //save original L
            for(int i = 0;i<3;i++) {                                    //actual denoise step
                buffer[0] = fimg[i];
                wavelet_denoise(buffer,width,height ,tres[i],low[i]);
            }
            for(int j=0;j<totalPixels;j++)                              //blend back in
                fimg[0][j]= BLEND(fimg[0][j],fimgOrg[j],fimgG[j]);
            if(doDePepper) for(int i = 0;i<3;i++) {                     //salt and pepepr removal
                buffer[0] = fimg[i];
                dePepper(buffer,width,height);
            }
        }
    }
    
    //Free memory
    for (int i = 1; i < NUMBUFFERS; i++) {
        delete[] buffer[i];
    }

    delete[] fimgG;
    delete[] fimgOrg;
}

