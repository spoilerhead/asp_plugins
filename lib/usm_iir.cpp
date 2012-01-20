/*

 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * based on the gimp IIR-USM code by
 *
 * Copyright (C) 1999 Winston Chang
 *                    <winstonc@cs.wisc.edu>
 *                    <winston@stdout.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "usm_iir.h"
#include <algorithm>
#include <cmath>

using namespace std;
          
inline float clip(const float x) {
//    return x;
    return std::max(std::min( x , 1.0f),0.0f);
}

inline int clipint(const int x,const int low,const int high) {
    if(x < low) return low;
    if(x >high) return high;
    return x;
}



typedef struct 
{
 float B,b1,b2,b3,b0,r,q;
 float *p;
} iir_param; 

static void iir_init(iir_param *iir, const float r)
{
  
  iir->r = r;
  float q;
  
  if ( r >= 2.5f) q = 0.98711f * r - 0.96330f;
  else q = 3.97156f - 4.14554f * sqrt(1.0f-0.26891f * r);
  
  iir->q = q;
  iir->b0 = 1.57825f + ((0.422205f * q  + 1.4281f) * q + 2.44413f) *  q;
  iir->b1 = ((1.26661f * q +2.85619f) * q + 2.44413f) * q / iir->b0;
  iir->b2 = - ((1.26661f*q +1.4281f) * q * q ) / iir->b0;
  iir->b3 = 0.422205f * q * q * q / iir->b0;
  iir->B = 1.0f - (iir->b1 + iir->b2 + iir->b3);

}

static void iir_filter(iir_param *iir, float *data, const int width, const int nextPixel)
/* 
 * Very fast gaussian blur with infinite impulse response filter
 * The row is blurred in forward direction and then in backward direction
 * So we achieve zero phase errors and symmetric impulse response
 * and good isotropy
 *
 * Theory for this filter can be found at:
 * <http://www.ph.tn.tudelft.nl/~lucas/publications/1995/SP95TYLV/SP95TYLV.pdf>
 * It is usable for radius downto 0.5. Lower radius must be done with the old
 * method. The old method also is very fast at low radius, so this doesnt matter 
 *
 * Double floating point precision is necessary for radius > 50, as my experiments
 * have shown. On my system (Duron, 1,2 GHz) the speed difference between double
 * and float is neglectable. 
 */

{

//  int w = radius;
 
  float *const lp = data, *const rp = data + ((width-1)*nextPixel);

//  int i;
 
  { //Forward loop
  /* Hoping compiler will use optimal alternative, if not enough registers */
    register float d1,d2,d3;
    data = lp;
    d1=d2=d3=*data; 
    while (data <=  rp){
      *data *=  iir->B;
      *data +=  iir->b3 * d3;      
      *data +=  iir->b2 * (d3 = d2);    
      *data +=  iir->b1 * (d2 = d1); 
      d1 = *data;
      *data = clip(*data);
      data += nextPixel;
    } 
  
  
  }
  
  data -=nextPixel;   
  { //backward loop
  /* Hoping compiler will use optimal alternative, if not enough registers */
    register float d1,d2,d3;
    d1=d2=d3=*data;
  
    while (data >=  lp){
      *data *=  iir->B;
      *data +=  iir->b3 * d3;      
      *data +=  iir->b2 * (d3 = d2);    
      *data +=  iir->b1 * (d2 = d1); 
      d1 = *data;
      *data = clip(*data);
      data -= nextPixel;
    } 
  
  }
}

void USM_IIR (float *fimg, const unsigned int width, const unsigned int height,const float radius)
{
    if(radius == 0.0f) return;
    unsigned int col, row;
    iir_param iir;
    iir_init(&iir,radius);
   
   
    //blur row
    for(row = 0; row < height; row++) { 
        iir_filter(&iir,fimg+(row*width), width, 1);
    }
    //blur col
    for(col = 0; col < width; col++) { 
        iir_filter(&iir,fimg+col, height, width);
    }
}


/*stacked version for larger radii*/
void USM_IIR_stacked (float *fimg, const unsigned int width, const unsigned int height,const float radius, const float stackRadius)
{
    if(radius == 0.0f) return;
    unsigned int col, row;
    iir_param iir;
   
    
    float remRadius = radius;
    while(remRadius > stackRadius) {
        iir_init(&iir,stackRadius);
        //blur row
        for(row = 0; row < height; row++) { 
            iir_filter(&iir,fimg+(row*width), width, 1);
        }
        //blur col
        for(col = 0; col < width; col++) { 
            iir_filter(&iir,fimg+col, height, width);
        }
        remRadius -= stackRadius;
    }

    iir_init(&iir,remRadius);
    //blur row
    for(row = 0; row < height; row++) { 
        iir_filter(&iir,fimg+(row*width), width, 1);
    }
    //blur col
    for(col = 0; col < width; col++) { 
        iir_filter(&iir,fimg+col, height, width);
    }
}


