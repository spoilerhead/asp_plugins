/*
 * Wavelet denoise GIMP plugin
 * 
 * wavelet.c
 * Parts Copyright 2008 by Marco Rossini
 * Parts copyright 1997-2010 by Dave Coffin (not GPLed!)
 * 
 * Implements the wavelet denoise code of UFRaw by Udi Fuchs
 * which itself bases on the code by Dave Coffin
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 * 
 * Ported to Bibble and modified by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010-2011
 */

#include "wavelet.h"

/*
 * code copied from UFRaw (which originates from dcraw) 
 */
inline void
hat_transform (float *temp, float *base, const int st, const int size,
               const int sc)
{
  int i;
  for(i = 0; i < sc; i++)
    temp[i] = 2 * base[st * i] + base[st * (sc - i)] + base[st * (i + sc)];
  for(; i + sc < size; i++)
    temp[i] = 2 * base[st * i] + base[st * (i - sc)] + base[st * (i + sc)];
  for(; i < size; i++)
    temp[i] = 2 * base[st * i] + base[st * (i - sc)]
      + base[st * (2 * size - 2 - (i + sc))];
}
//#define USE_UFRAW_CODE
#ifdef USE_UFRAW_CODE
/*
 * actual denoising algorithm. code copied from UFRaw (originates from
 * dcraw) 
 */
void
wavelet_denoise (float *fimg[3],
                 const unsigned int width, const unsigned int height,
                 const float threshold, const float low)
{
  float *temp, thold;
  unsigned int i, lev, lpass, hpass, size, col, row;
  float stdev[10];
  unsigned int samples[10];

  size = width * height;
  temp = new float[(max (width, height))];
                                      
  if(threshold >0.0f) {
      hpass = 0;
      for(lev = 0; lev < 5; lev++) {
        lpass = ((lev & 1) + 1);
        for(row = 0; row < height; row++) {
          hat_transform (temp, fimg[hpass] + row * width, 1, width, 1 << lev);
          for(col = 0; col < width; col++) {
            fimg[lpass][row * width + col] = temp[col] * 0.25f;
          }
        }
        for(col = 0; col < width; col++) {
          hat_transform (temp, fimg[lpass] + col, width, height, 1 << lev);
          for(row = 0; row < height; row++) {
            fimg[lpass][row * width + col] = temp[row] * 0.25f;
          }
        }

        
        //thold = 5.0 / (1 << 6) * /*noise[lev];*/exp (-2.6 * sqrt (lev + 1)) * 0.8002 /      exp (-2.6);*/
        // replaced by lookup table
        static const float noiset[] = { 0.0625156250000000f,	 0.0212947649034830f,
        	                            0.00931927117766594f, 0.00464325916305547f,
        	                            0.00251340845961395f, 0.00144302729383771f,
        	                            0.000866288848032195f,0.000538755192886314f};
        thold = noiset[lev];
          

        // initialize stdev values for all intensities 
        for(i = 0; i < 10; i++) {
          stdev[i] = 0.0f;
          samples[i] = 0;
        }

        // calculate stdevs for all intensities 
        for(i = 0; i < size; i++) {
          fimg[hpass][i] -= fimg[lpass][i];
          if(fimg[hpass][i] < thold && fimg[hpass][i] > -thold) {
            int sdev = max (min (9, int (fimg[lpass][i] * 10)), 0);
            stdev[sdev] += fimg[hpass][i] * fimg[hpass][i];
            samples[sdev]++;
          }
        }

        for(i = 0; i < 10; i++) {
          stdev[i] = sqrt (stdev[i] / (samples[i] + 1));
        }

        //
        //  do thresholding 
        // 
        for(i = 0; i < size; i++) {
          int sdev = max (min (9, int (fimg[lpass][i] * 10)), 0);
          thold = threshold * stdev[sdev];

          if(fimg[hpass][i] < -thold)
            fimg[hpass][i] += thold - thold * low;
          else if(fimg[hpass][i] > thold)
            fimg[hpass][i] -= thold - thold * low;
          else
            fimg[hpass][i] *= low;

          if(hpass)
            fimg[0][i] += fimg[hpass][i];
        }/* this would be dcraw's fixed implementation
        for (i=0; i < size; i++) {
	        fimg[hpass][i] -= fimg[lpass][i];
	        if	(fimg[hpass][i] < -thold) fimg[hpass][i] += thold;
	        else if (fimg[hpass][i] >  thold) fimg[hpass][i] -= thold;
	        else	 fimg[hpass][i] = 0;
	        if (hpass) fimg[0][i] += fimg[hpass][i];
	    }*/
        hpass = lpass;
      }

    
    
    
      for(i = 0; i < size; i++) 
        fimg[0][i] = fimg[0][i] + fimg[lpass][i];
  }  
    
  delete[] temp;
}





#else
//======================
/*
 * actual denoising algorithm. Ported code from dcraw
 */
void
wavelet_denoise (float *fimg[3],
                 const unsigned int width, const unsigned int height,
                 const float threshold, const float low)
{
  float *temp, thold;
  unsigned int i, lev, lpass, hpass, size, col, row;


  size = width * height;
  temp = new float[(max (width, height))];
                                      
  if(threshold >0.0) {
      hpass = 0;
      for(lev = 0; lev < 5; lev++) {
        lpass = ((lev & 1) + 1);
        for(row = 0; row < height; row++) {
          hat_transform (temp, fimg[hpass] + row * width, 1, width, 1 << lev);
          for(col = 0; col < width; col++) {
            fimg[lpass][row * width + col] = temp[col] * 0.25f;
          }
        }
        for(col = 0; col < width; col++) {
          hat_transform (temp, fimg[lpass] + col, width, height, 1 << lev);
          for(row = 0; row < height; row++) {
            fimg[lpass][row * width + col] = temp[row] * 0.25f;
          }
        }

        
        //thold = 5.0 / (1 << 6) * /*noise[lev];*/exp (-2.6 * sqrt (lev + 1)) * 0.8002 /      exp (-2.6);*/
        // replaced by lookup table
        static const float noiset[] =  { 0.0625156250000000f ,	 0.0212947649034830f  ,
        	                            0.00931927117766594f ,   0.00464325916305547f ,
        	                            0.00251340845961395f ,   0.00144302729383771f ,
        	                            0.000866288848032195f,   0.000538755192886314f};
        thold = threshold* noiset[lev];
          
        for (i=0; i < size; i++) {
	        fimg[hpass][i] -= fimg[lpass][i];
	        if	(fimg[hpass][i] < -thold) 
	                fimg[hpass][i] += thold - thold * low;
	            else if (fimg[hpass][i] >  thold) 
	                fimg[hpass][i] -= thold - thold * low;
	            else
	                fimg[hpass][i] *= low;
	        if (hpass) fimg[0][i] += fimg[hpass][i];
        }
        hpass = lpass;
    
      }
    
    
      for(i = 0; i < size; i++) 
        fimg[0][i] = fimg[0][i] + fimg[lpass][i];
  }  
    
  delete[] temp;
}

#endif
