#include "sph_wavelet_denoise.h"
//	If this is defined the plugin will defer the alpha blending to the Bibble 5 application.
//	When it is not defined the plugin will do its own alpha blending for Layers.
#define _BIBBLE_BLENDING_

BDebugTraceFunction BTestDebug;


// THIS IS A TESTING VERSION!
// don't use for anything valuable


#define colorf 65536.0f  //to scale down
#define DO_DENOISE    //disable sharpening alltogether for debug purpose

//#define DO_RGBSPACE  //if off we use lab space

/* Wavelet denoise bibble plugin
 * Copyright 2010 by Dieter Steiner <spoilerhead@gmail.com>
 * based on 
 *
 * Wavelet denoise GIMP plugin
 * 
 * wavelet.c
 * Copyright 2008 by Marco Rossini
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2+
 * as published by the Free Software Foundation.
 *
 *
 *
 * http://registry.gimp.org/node/4235
 */

/* code copied from UFRaw (which originates from dcraw) */


inline float clip (const float i, const float imin, const float imax) {
  return max(min(imax,i),imin);
}


#ifdef DO_RGBSPACE
inline void rgb2ycbcr (float *r, float *g, float *b)
{
  /* using JPEG conversion here - expecting all channels to be
   * in [0:255] range */
  float y, cb, cr;
  y = 0.2990 * (*r) + 0.5870 * (*g) + 0.1140 * (*b);
  cb = -0.1687 * (*r) - 0.3313 * (*g) + 0.5000 * (*b) + 0.5;
  cr = 0.5000 * (*r) - 0.4187 * (*g) - 0.0813 * (*b) + 0.5;
  *r = y;
  *g = cb;
  *b = cr;
}

inline void ycbcr2rgb (float *y, float *cb, float *cr)
{
  /* using JPEG conversion here - expecting all channels to be
   * in [0:255] range */
  float r, g, b;
  r = (*y) + 1.40200 * ((*cr) - 0.5);
  g = (*y) - 0.34414 * ((*cb) - 0.5) - 0.71414 * ((*cr) - 0.5);
  b = (*y) + 1.77200 * ((*cb) - 0.5);
  *y = r;
  *cb = g;
  *cr = b;
}
#endif


// typedef int (*PipeFunctionPtr )(struct BibblePluginData *data,struct PipeData *pipeData)
int ProcessImage(struct BibblePluginData *bibbleData, struct BPluginPipeData *pipeData) {
	// This function inverts the image 
	int rc = 0;
	// We don't need to check if we're on anymore, because Bibble will use the runCheck result

	// Grab the image data and info
	BImageHandle bimg, dest, alpha = 0;
	
	//--------
	
	
	if ( pipeData->m_layerNum != 0 )
	{
		bimg = dest = BTileImage(pipeData->m_tile);
		alpha = pipeData->m_alpha;
#ifdef _BIBBLE_BLENDING_
		//	When Bibble does the blending our destination buffer must be pipeData->m_dest
		dest = pipeData->m_dest;
		pipeData->m_blend = true;
#endif
	}
	else
	{
		//	Never any alpha blending for the main layer
		bimg = dest = BTileImage(pipeData->m_tile);
		alpha = 0;
	}

	// If we don't have an image, we can't do anything
	if ( bimg == 0 || dest == 0 )
	{
		BMSG( "ProcessImage() failed - NULL image");
		return -1;
	}

	int w,h,nChan,pw,rw,padw,padh,np;

	w = BImageWidth(bimg);
	h = BImageHeight(bimg);
	nChan = BImageNumChannels(bimg);
	pw = BImagePlaneWords(bimg);	// plane words - how far to get to the next plane
	rw = BImageRowWords(bimg);		// number of unsigned shorts in a row (includes padding)
	padw = bibbleData->m_bif.m_padWidth( bimg );
	padh = BImagePadLines(bimg);
	np = BImageNextPixel(bimg);		// increment to get to the next pixel


    //Corner Points
    BPoint topLeft,bottomRight;
    BPoint topLeftNoPad,bottomRightNoPad;
    topLeft.x = -padw;
    topLeft.y= -padh;
    bottomRight.x = w+padw;
    bottomRight.y = h+padh;
    
    //topLeftNoPad.x = 0;
    //topLeftNoPad.y= 0;
    //bottomRightNoPad.x = w;
    //bottomRightNoPad.y = h;


	unsigned short *pIn, *pOut, *pEnd;//, *pAlpha;
	int px_i;   //pixel index

    // Read channel Settings
	int tmp;
	float threshold[3]; //channel specific threshold
	bibbleData->m_bof.m_getOptionInt( pipeData->m_options,WAVELET_TRES_L, bibbleData->m_plugin, tmp);
	threshold[0] = tmp/10.0;
	bibbleData->m_bof.m_getOptionInt( pipeData->m_options,WAVELET_TRES_A, bibbleData->m_plugin, tmp);
	threshold[1] = tmp/10.0;
	bibbleData->m_bof.m_getOptionInt( pipeData->m_options,WAVELET_TRES_B, bibbleData->m_plugin, tmp);
	threshold[2] = tmp/10.0;	
	
	 
	float low[3];
	bibbleData->m_bof.m_getOptionInt( pipeData->m_options,WAVELET_LOW_L, bibbleData->m_plugin, tmp);
	low[0] = tmp/100.0;
	bibbleData->m_bof.m_getOptionInt( pipeData->m_options,WAVELET_LOW_A, bibbleData->m_plugin, tmp);
	low[1] = tmp/100.0;
	bibbleData->m_bof.m_getOptionInt( pipeData->m_options,WAVELET_LOW_B, bibbleData->m_plugin, tmp);
	low[2] = tmp/100.0;
	
	bool doDePepper;
	bibbleData->m_bof.m_getOptionBool( pipeData->m_options,  WAVELET_DEPEPPER, bibbleData->m_plugin, doDePepper);
	
    /* //print param set
    for (c = 0; c < ((nChan % 2) ? nChan : nChan - 1); c++)
    {
        char buf[50]; 
        sprintf(buf,"c: %d  t: %f  l:%f",c,threshold[c],low[c]);
        BTestDebug(buf);
    }*/    


    int channels_denoised, c;
    

        
    //BTestDebug("preparing");
	//	Always do this for the main layer or if Bibble is doing the blending for us.
	if ( pipeData->m_layerNum == 0 || pipeData->m_blend )
	{

        int i, size;
        int height = bottomRight.y-topLeft.y;//h+2*padh;
        int width = bottomRight.x-topLeft.x;//w+2*padw;
        size= height*width;
        
        //BTestDebug("allocating Buffers...");
       
        //Allocate channel buffer
        float *fimg[3];//[nChan];
        for (i = 0; i < nChan; i++)
        {
            fimg[i] = (float *)( bibbleData->m_bmf.m_alloc((height) * (width) * sizeof (float)) );
        }
        
        //allocate Wavelet buffer
        float *buffer[3];
        for (i = 1; i < 3; i++) //don't alloc buffer 0
        {
            buffer[i] = (float *)( bibbleData->m_bmf.m_alloc((height) * (width) * sizeof (float)) );
        }
        
        //BTestDebug("OK");
        
        
        //BTestDebug("converting pixels...");
        //Convert 0.65535 pixels into 0.1 and colorspace transformation if needed
        px_i = 0;
        for(int y = topLeft.y; y < bottomRight.y; ++y ) 
        {
			pIn = BImageScanLine16( bimg, 0 ) + y*rw + (topLeft.x*np);
			pEnd = pIn + (width)*np;
			while( pIn < pEnd ) 
			{
				fimg[0][px_i] = *(pIn)       /colorf; //L || R
				fimg[1][px_i] = *(pIn + pw)  /colorf; //A || B
				fimg[2][px_i] = *(pIn + 2*pw)/colorf; //B || G
				pIn += np;
				
				#ifdef DO_RGBSPACE
				if (nChan > 2 )
    				rgb2ycbcr (&(fimg[0][px_i]), &(fimg[1][px_i]), &(fimg[2][px_i]));
                #endif
                
				px_i++;
			}
		}
	   //BTestDebug("OK");
         
                     
#ifdef DO_DENOISE       
//        BTestDebug("Denoising...");
        channels_denoised = 0;
        for (c = 0; c < ((nChan % 2) ? nChan : nChan - 1); c++)
        {
          buffer[0] = fimg[c];
          if (threshold[c] > 0 /*|| doDePepper*/) //Do DePepper only when threshold >0 to avoid artifacting
	      {
                 wavelet_denoise(bibbleData, buffer,width,height,threshold[c],low[c],doDePepper);
	      }
	      channels_denoised++;
        }	   
//        BTestDebug("Denoise done");	   
#endif //DO_DENOISE 
        	   
	   
	   
	   //BTestDebug("Copy to output...");
	   //Retransform into bibble colorspace
	   px_i = 0;
       for(int y = topLeft.y; y < bottomRight.y; ++y ) 
       {
			pOut = BImageScanLine16( dest, 0 ) + y*rw + (topLeft.x*np);
			pEnd = pOut + (width)*np;
			while( pOut < pEnd ) 
			{			
				#ifdef DO_RGBSPACE
				if (nChan > 2)
				    ycbcr2rgb (&(fimg[0][px_i]), &(fimg[1][px_i]), &(fimg[2][px_i]));
				#endif
				
				*(pOut) =        clip(fimg[0][px_i]*colorf,0,colorf-1); //L || R
				*(pOut + pw) =   clip(fimg[1][px_i]*colorf,0,colorf-1); //A || G
				*(pOut + 2*pw) = clip(fimg[2][px_i]*colorf,0,colorf-1); //B || B
				
				pOut += np;
				px_i++;
			}
		}
		//BTestDebug("OK");
		
		//BTestDebug("deleting buffers...");
		for (i = 0; i < nChan; i++)
        {
	        bibbleData->m_bmf.m_free((BMemoryHandle)fimg[i]);
    	}
	    for (i = 1; i < 3; i++)
        {
	        bibbleData->m_bmf.m_free((BMemoryHandle)buffer[i]);
	    }
		//BTestDebug("OK");
	   
	}

	return rc;
}


// You may name the function anything you like, but if you want to use the macros to
// call the Bibble functions, you must keep the parameter names the same
// typedef int	(*OptionsInitPtr )(struct BibblePluginData *bibbleData, struct OptionData *od);

int initializeOptions(struct  BibblePluginData *bibbleData, BSettingsWidgetHandle sw)
{
	// This function is called when the plugin loads
	int rc = 0;

	return rc;
}

/*
  The following function is called in order to fix dependancies between
  settings (for instance, if some user has to choose a color with three
  sliders (red, green, blue), you may want to change sliders for some
  Hue, saturation, value model of the same color (HSV) in order to
  allow the user to choose a color by two different (but linked) ways.
  You may see some dependancies with the Simple Color Filter v1.1 (just
  set the color with the sliders, and see the color of the button changing
  in the same time).
  The most known feature handled here is enabling the plugin when the user
  moves a slider.
  //taken from bathsheba
*/
__declspec(dllexport) int doDependancies( struct BibblePluginData *bibbleData, struct OptionData *od) {
	
	int rc = 0;

	if (
	   (BOptionListContains(od->m_changeList,WAVELET_TRES_L,bibbleData->m_plugin)) 
	|| (BOptionListContains(od->m_changeList,WAVELET_LOW_L,bibbleData->m_plugin)) 
	|| (BOptionListContains(od->m_changeList,WAVELET_TRES_A,bibbleData->m_plugin)) 
	|| (BOptionListContains(od->m_changeList,WAVELET_LOW_A,bibbleData->m_plugin)) 
	|| (BOptionListContains(od->m_changeList,WAVELET_TRES_B,bibbleData->m_plugin)) 
	|| (BOptionListContains(od->m_changeList,WAVELET_LOW_B,bibbleData->m_plugin))
	)
	{
		BSetOptionBool(od->m_changeList,WAVELET_ENABLED,bibbleData->m_plugin,true);
		return 1;
	}
	BTestDebug("Wavelet Denoise - Do Dependancies");

	return rc;
}




__declspec(dllexport) int RegisterPlugin(struct BibblePluginData *bibbleData)
{
	// This function is called by Bibble during loading
	// Plug-ins should register the functions Bibble needs to call,
	// check that they are compatible with the version of Bibble attempting to load them,
	// add their settings to the factory defaults list for the Bibble option system,
	// and allocate and initialize any global data or resources
	int rc = 0;

	BTestDebug = (BDebugTraceFunction)(bibbleData->m_buf.m_trace);

	// Set the API version we were compiled under
	// This is defined in BibblePlugin.h
	bibbleData->m_version = BIBBLE5PLUGIN_API_VERSION;
	bibbleData->m_bsf.m_fromAscii( bibbleData->m_groupName, WAVELETDENOISE_PLUGINGROUP);
	bibbleData->m_bsf.m_fromAscii( bibbleData->m_pluginName, WAVELETDENOISE_PLUGINNAME);

	//work in lab space
	bibbleData->m_brf.m_registerPipeRoutines(bibbleData->m_plugin, ProcessImage,  PIPE_PLUGIN_SHARPEN , 199);
    bibbleData->m_brf.m_registerOptionsInit(bibbleData->m_plugin, initializeOptions);
    
    // The function registered below (doDependancies) will be called
    // when a setting will be changed
    //TODO: doesn't work, no idea why, it works in the tutorial plugin
	bibbleData->m_brf.m_registerSettingsWidgetDoDependancies(bibbleData->m_plugin, doDependancies );
	
    BMSG("Wavelet Denoise plug-in loaded - running in LAB space\n");
	return rc;
}

__declspec(dllexport) int unRegisterPlugin(struct BibblePluginData *bibbleData)
{
	// This function is called by Bibble right before the program exits
	// Plug-ins should free any global memory and resources here
	int rc = 0;

	return rc;
}

__declspec(dllexport) int getVersion( void )
{
	return BIBBLE5PLUGIN_API_VERSION;
}

__declspec(dllexport) int buildDefaults(struct BibblePluginData *bibbleData, struct BAddOptionFunctions *baf)
{
	int rc = 0;
	baf->m_addBoolOption( bibbleData->m_plugin, WAVELET_ENABLED,"bSphWaveletDenoiseon",     "Enable", WAVELETDENOISE_GROUP, "Enable Wavelet Denoise", false, OPTION_RESTART|OPTION_NO_DEPENDENCY);
	
	baf->m_addIntOption( bibbleData->m_plugin, WAVELET_TRES_L,  "bSphWaveletDenoiseThresL", "Threshold L", WAVELETDENOISE_GROUP, "Thres L", 0, OPTION_RESTART);
	baf->m_addIntOption( bibbleData->m_plugin, WAVELET_LOW_L,   "bSphWaveletDenoiseLowL",   "Low L", WAVELETDENOISE_GROUP, "Low L", 10, OPTION_RESTART);
	
    baf->m_addIntOption( bibbleData->m_plugin, WAVELET_TRES_A,  "bSphWaveletDenoiseThresA", "Threshold A", WAVELETDENOISE_GROUP, "Thres A", 10, OPTION_RESTART);
	baf->m_addIntOption( bibbleData->m_plugin, WAVELET_LOW_A,   "bSphWaveletDenoiseLowA",   "Low A", WAVELETDENOISE_GROUP, "Low A", 0, OPTION_RESTART);
	
	baf->m_addIntOption( bibbleData->m_plugin, WAVELET_TRES_B,  "bSphWaveletDenoiseThresB", "Threshold B", WAVELETDENOISE_GROUP, "Thres B", 10, OPTION_RESTART);
	baf->m_addIntOption( bibbleData->m_plugin, WAVELET_LOW_B,   "bSphWaveletDenoiseLowB",   "Low B", WAVELETDENOISE_GROUP, "Low B", 0, OPTION_RESTART);	
	
	baf->m_addBoolOption( bibbleData->m_plugin, WAVELET_DEPEPPER,"bSphWaveletDenoiseDePepper",     "Depepper", WAVELETDENOISE_GROUP, "Depepper", false, OPTION_RESTART);
	
	
	
	return rc;
}

__declspec(dllexport) bool runCheck(struct BibblePluginData *bibbleData, struct BPluginPipeData *pipeData, BBOOL &needPrev )
{
	bool enabled = false;
	needPrev = false;
	bibbleData->m_bof.m_getOptionBool( pipeData->m_options, WAVELET_ENABLED, bibbleData->m_plugin, enabled);

	return enabled;
}
