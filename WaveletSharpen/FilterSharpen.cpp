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

 
 
#include "FilterSharpen.h"
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

#define FilterName WaveletSharpenFilter






bool FilterName::needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	bool okay;
	bool val    = layerOptions.getBool(Enable, m_groupId, okay);
	bool val2   = layerOptions.getBool(Enable2, m_groupId, okay);

	
	bool retVal =  val || val2;
	#ifdef INSTRUMENTATION
	qDebug()<<name()<<"run? 1:"<<val<<" 2: "<<val2<<" Return value:"<<retVal;
	#endif
	
	return retVal;
}

bool FilterName::isSlow(const ImageSettings &options, const PipeSettings  &settings) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	return false;//true;
}



bool FilterName::needsOriginalImage  ()
{
	return true; //we only use the current tile
}

int FilterName::additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const
{
    bool okay;
    bool  enable  = layerOptions->getBool  (Enable,  m_groupId, okay);
    float radius  = layerOptions->getDouble(Radius,  m_groupId, okay)*zoomLevel;
    bool  enable2 = layerOptions->getBool  (Enable2, m_groupId, okay);
	float radius2 = layerOptions->getDouble(Radius2, m_groupId, okay)*zoomLevel;

  	
	float effRadius = ((enable?(radius):0.0f) + (enable2?(radius2):0.0f));
	
    return (effRadius)*5.0f;
}

/*LUT based ys=1./(1+exp(-(20*x-1))).*(x.^(1/5)); there x is in [0,1]
    i'm, not sure if 256 isn't to much, but it works
*/
//this table is about the same as x^(1/5) for [0.3 1.0] but has a faster falloff in [0 0.3]
static const float sigmoidtable[513] = {
    #include "edgetable.inc"
};

static const float edgeFunction(const float x)
{

    #define MMAX(a,b) ((a>b)?a:b)
    #define MMIN(a,b) ((a<b)?a:b)
    #define MCLIP(a) (MMAX(MMIN(1.0f,a),0.0f))
    /*int i = MCLIP(x)*512;
    return(sigmoidtable[i]);*/
    
  // Look up and interpolate
  float lookup = clipf(x * 512.0f, 0.0f, 511.9999f);
  float v0 = sigmoidtable[(int)lookup];
  float v1 = sigmoidtable[(int)lookup + 1];
  lookup -= floorf(lookup);
  return v0 * (1.0f - lookup) + v1 * lookup;
}

void FilterName::ProcessBuffer(pixel *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
    Q_UNUSED(tile);
    Q_UNUSED(layerPos);
    
    const int totalPixels = width*height;
        //Make a copy of the original L channel
        pixel *org = new pixel[width*height];
        memcpy(org,fimg[0],width*height*sizeof(pixel));
        
        #define NUMBUFFERS 3
        pixel *buffer[NUMBUFFERS];
        for (int i = 1; i < NUMBUFFERS; i++) //don't alloc buffer 0
        {
            buffer[i] = new pixel[width*height];
        }
       
        //------
        
        
        bool okay;

        //STEP 1
	    bool  enable = layerOptions.getBool(Enable, m_groupId, okay);
	    float amount = layerOptions.getInt(Amount, m_groupId, okay)/10.0f*(zoomLevel);//*zoomLevel ;//= 1.5; //0..10
	    float radius = layerOptions.getDouble(Radius, m_groupId, okay)*zoomLevel;///10.0f; //= 0.5; //0..2
	   // float blend  = layerOptions.getInt(Blend, m_groupId, okay)/100.0f;/*zoomLevel*/;
	    bool doDePepper = layerOptions.getBool(DePepper, m_groupId, okay);
	    float edges  = layerOptions.getInt(Edges, m_groupId, okay)/100.0f*zoomLevel; //Edge blending
	    bool enableEdges = layerOptions.getInt(Edges, m_groupId, okay) != 0; //if not 0, edge sensitivityis enabled
	    bool clarity = layerOptions.getBool(Clarity, m_groupId, okay); 
	    //STEP 2
	    bool  enable2 = layerOptions.getBool(Enable2, m_groupId, okay);
	    float amount2 = layerOptions.getInt(Amount2, m_groupId, okay)/10.0f*(zoomLevel);//*zoomLevel ;//= 1.5; //0..10
	    float radius2 = layerOptions.getDouble(Radius2, m_groupId, okay)*zoomLevel;///10.0f; //= 0.5; //0..2
	    //float blend2  = layerOptions.getInt(Blend2, m_groupId, okay)/100.0f;/*zoomLevel*/;
	    bool doDePepper2 = layerOptions.getBool(DePepper2, m_groupId, okay);
	    float edges2  = layerOptions.getInt(Edges2, m_groupId, okay)/100.0f*zoomLevel; //Edge blending
	    bool enableEdges2 = layerOptions.getInt(Edges2, m_groupId, okay) != 0; //if not 0, edge sensitivityis enabled
	    bool clarity2 = layerOptions.getBool(Clarity2, m_groupId, okay); 
           

        //---------------------------------------------------------------------------------------------

        #ifdef INSTRUMENTATION
	    qDebug()<<name()<<"process: 1:"<<enable<<" 2: "<<enable2;
    	#endif
        
     
        //---------------------------------------------------------------------------------------------        
                
        //Wavelet Sharpening Stage
        //---------------------------------------------------------------------------------------------        
        buffer[0] = fimg[0];
        if(enable) {
            #ifdef BLEND_LAST_STEP
            memcpy(org,fimg[0],totalPixels*sizeof(pixel));
            #endif
            wavelet_sharpen(buffer,width,height ,amount,radius);
            if(doDePepper) dePepper(buffer,width,height);
            
           
            //-- Edge awareness (smart sharpening) -------------
            if(enableEdges) {
                //create a usm for the edge aware sharpening
                #ifdef USE_USM_EDGEDETECTION

                    memcpy(buffer[1],org,totalPixels*sizeof(pixel));
                    USM_IIR(buffer[1],width,height,radius*2+0.5f);      //blur

                #endif
            
                pixel *diffBuf = buffer[2];
                for(int px_i=0;px_i<totalPixels;px_i++) {               // Calculate diff alues
                    float diff = 1.0f;
                    diff = fabs((buffer[1][px_i]-org[px_i]));           // use a simple usm as mask,
                    diff = edgeFunction(diff);                          // apply the edge function
                    diff = (edges*diff)+(1.0f-edges);                   // Tone down diff based on slider
                    diffBuf[px_i]=diff;
                } 
                
                for(int px_i=0;px_i<totalPixels;px_i++) {               // Do the blending
                    fimg[0][px_i]   = BLEND(fimg[0][px_i], org[px_i], diffBuf[px_i]); //Blend with Original
                }

            } //------------------------------------------------
            if(clarity) {
                for(int px_i=0;px_i<totalPixels;px_i++) { 
                    float clarity =  (
                                1.f-fabs(2.f*(org[px_i]-0.5f))      //1 at mid gray, 0 at 0 and 1
                           );
                    fimg[0][px_i]   = BLEND(fimg[0][px_i], org[px_i], clarity); //Blend with Original
                }
            }
        }
        
        //---------------------------------------------------------------------------------------------        
        if(enable2) {
            #ifdef BLEND_LAST_STEP
            memcpy(org,fimg[0],totalPixels*sizeof(pixel));
            #endif
            wavelet_sharpen(buffer,width,height ,amount2,radius2);
            if(doDePepper2) dePepper(buffer,width,height);
            
           
            //-- Edge awareness (smart sharpening) -------------
            if(enableEdges2) {
                //create a usm for the edge aware sharpening
                #ifdef USE_USM_EDGEDETECTION

                    memcpy(buffer[1],org,totalPixels*sizeof(pixel));
                    USM_IIR(buffer[1],width,height,radius2*2+0.5f);      //blur

                #endif
            
                pixel *diffBuf = buffer[2];
                for(int px_i=0;px_i<totalPixels;px_i++) {               // Calculate diff alues
                    float diff = 1.0f;
                    diff = fabs((buffer[1][px_i]-org[px_i]));           // use a simple usm as mask,
                    diff = edgeFunction(diff);                          // apply the edge function
                    diff = (edges2*diff)+(1.0f-edges2);                   // Tone down diff based on slider
                    diffBuf[px_i]=diff;
                } 
                
                for(int px_i=0;px_i<totalPixels;px_i++) {               // Do the blending
                    fimg[0][px_i]   = BLEND(fimg[0][px_i], org[px_i], diffBuf[px_i]); //Blend with Original
                }
            } //------------------------------------------------

            if(clarity2) {
                for(int px_i=0;px_i<totalPixels;px_i++) { 
                    float clarity =  (
                                1.f-fabs(2.f*(org[px_i]-0.5f))      //1 at mid gray, 0 at 0 and 1
                           );
                    fimg[0][px_i]   = BLEND(fimg[0][px_i], org[px_i], clarity); //Blend with Original
                }
            }
        }
        //---------------------------------------------------------------------------------------------        
        delete[] org;
        //Free memory
	    for (int i = 1; i < NUMBUFFERS; i++) {
            delete[] buffer[i];
        }
}

