/*
 * Wavelet sharpen GIMP plugin
 * 
 * wavelet.c
 * Copyright 2008 by Marco Rossini
 * 
 * Implements the wavelet denoise code of UFRaw by Udi Fuchs
 * which itself bases on the code by Dave Coffin
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 * 
 * Ported to Bibble and modified by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 */

#include "wavelet.h"
#include "colorspace.h"


/*
 * code copied from UFRaw (which originates from dcraw) 
 
 It is simply the implementation of the h = (0.25,0.5,0.25) linear 
Lagrange interpolation (as I thought), with some hocus-pocus at the edges.
So the complete algorithm is "A trous" algorithm with a linear 
interpolation filter.
http://www.mail-archive.com/ufraw-devel@lists.sourceforge.net/msg00476.html


 */
static inline void hat_transform (float *temp, float *base, const int st, const int size, const int sc)
{
  int i;
  for (i = 0; i < sc; i++)
    temp[i] = 2 * base[st * i] + base[st * (sc - i)] + base[st * (i + sc)];
  for (; i + sc < size; i++)
    temp[i] = 2 * base[st * i] + base[st * (i - sc)] + base[st * (i + sc)];
  for (; i < size; i++)
    temp[i] = 2 * base[st * i] + base[st * (i - sc)]
      + base[st * (2 * size - 2 - (i + sc))];
}
/*
 * actual denoising algorithm. code copied from UFRaw (originates from
 * dcraw) 
 */
void
wavelet_sharpenF (float *fimg[3],
                 const unsigned int width, const unsigned int height,
                 const float amount, const float radius)
{
  float *temp;
  unsigned int i, lev, lpass, hpass, size, col, row;

  size = width * height;
  temp = new float[(max (width, height))];
                                      
  if(amount > 0.0f) {
    hpass = 0;
    lpass = 0;
    for (lev = 0; lev < 5; lev++)
    {
        lpass = ((lev & 1) + 1);
        //horizontal
        for (row = 0; row < height; row++) {
          //target, source, step,how_many_steps,
          hat_transform (temp, fimg[hpass] + row * width, 1, width, 1 << lev);
          for (col = 0; col < width; col++) {
              fimg[lpass][row * width + col] = temp[col] * 0.25f;
          }
        }
        
        for (col = 0; col < width; col++) {
          hat_transform (temp, fimg[lpass] + col, width, height, 1 << lev);
          for (row = 0; row < height; row++) {
              fimg[lpass][row * width + col] = temp[row] * 0.25f;
          }
        }
        
        float amt = amount * exp (-(lev - radius) * (lev - radius) / 1.5f) + 1;
        /*for (i = 0; i < size; i++)
        {
          fimg[hpass][i] -= fimg[lpass][i];
          fimg[hpass][i] *= amt;

          if (hpass)
            fimg[0][i] += fimg[hpass][i];
        }*/
        //sph removed branchpoint from loop
        if (hpass)
          for (i = 0; i < size; i++) {
              fimg[hpass][i] -= fimg[lpass][i];
              fimg[hpass][i] *= amt;
              fimg[0][i] += fimg[hpass][i];
        } else for (i = 0; i < size; i++) {
              fimg[hpass][i] -= fimg[lpass][i];
              fimg[hpass][i] *= amt;
        } 
        
        hpass = lpass;
    }
    

    for (i = 0; i < size; i++) {
       fimg[0][i] = fimg[0][i] +fimg[lpass][i];
       
       
    }
      

  }  
  
  
  delete[] temp;
}




#if 0
// TEST, integer =====================================================================================================

/*
 * code copied from UFRaw (which originates from dcraw) 
 */
inline void hat_transformI (int *temp, int *base, const int st, const int size, const int sc)
{
  int i;
  for (i = 0; i < sc; i++)
    temp[i] = 2 * base[st * i] + base[st * (sc - i)] + base[st * (i + sc)];
  for (; i + sc < size; i++)
    temp[i] = 2 * base[st * i] + base[st * (i - sc)] + base[st * (i + sc)];
  for (; i < size; i++)
    temp[i] = 2 * base[st * i] + base[st * (i - sc)]
      + base[st * (2 * size - 2 - (i + sc))];
}

/*
 * actual denoising algorithm. code copied from UFRaw (originates from
 * dcraw) 
 */
void
wavelet_sharpenI (int *fimg[3],
                 const unsigned int width, const unsigned int height,
                 const float amount, const float radius)
{
  int *temp;
  unsigned int i, lev, lpass, hpass, size, col, row;

  size = width * height;
  temp = new int[(max (width, height))];
                                      
  if(amount >0.0f) {
               hpass = 0;
               lpass=0;
               //BTestDebug("Wavelet..."); 
                for (lev = 0; lev < 5; lev++)
                {
                    
                    lpass = ((lev & 1) + 1);
                    //horizontal
                    //BTestDebug("Loop1");
                    
                    
                    //sprintf(buf,"c: %d lev: %d  lpass: %d hpass: %d",c,lev,lpass,hpass);
                    //BTestDebug(buf);
                    for (row = 0; row < height; row++)
                    {
                      //target, source, step,how_many_steps,
                      hat_transformI (temp, fimg[hpass] + row * width, 1, width, 1 << lev);
                      for (col = 0; col < width; col++)
                        {
        //                  char buf[50];
        //                  sprintf(buf,"%d  %d %d %d",lev,lpass, row,col);
        //                  BTestDebug(buf);
                          fimg[lpass][row * width + col] = temp[col] >>2;
                        }
                    }
                    
                    //BTestDebug("Loop2");
                    
                    for (col = 0; col < width; col++)
                    {
                      hat_transformI (temp, fimg[lpass] + col, width, height, 1 << lev);
                      for (row = 0; row < height; row++)
                        {
                          fimg[lpass][row * width + col] = temp[row] >>2;
                        }
                    }
                    
                     float amt = amount * exp (-(lev - radius) * (lev - radius) / 1.5f) + 1;
                    //BTestDebug("Loop3");
                    //sprintf(buf,"amt: %f ",amt);
                    //BTestDebug(buf);
                    for (i = 0; i < size; i++)
                    {
                      fimg[hpass][i] -= fimg[lpass][i];
                      fimg[hpass][i] *= amt;

                      if (hpass)
                        fimg[0][i] += fimg[hpass][i];
                    }
                    hpass = lpass;
                }
                
                //BTestDebug("Final step");
                for (i = 0; i < size; i++)
                   fimg[0][i] = fimg[0][i] + fimg[lpass][i];    
        	   //BTestDebug("OK");
  }  
  
  
  delete[] temp;
}
#endif

