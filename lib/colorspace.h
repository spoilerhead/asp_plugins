/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
 
#ifndef _COLORSPACE_H
#define _COLORSPACE_H


#include <algorithm>
#include "fastmath.h"
#include "pstdint.h"


#define COLORSPACE_FASTLOCAL static inline


#define WHITEf 1.0f
#define BLACKf 0.0f

#define WHITEi 65535
#define BLACKi 0

#define f_itof 65535.0f  //float to int scale factor
#define I16TOF(v) ((v)*(1.f/f_itof))
#define FTOI16(v) ( clipi ((v) * f_itof,BLACKi,WHITEi) )//add clip

#define I16TOI32(v) ((v))
#define I32TOI16(v) ( clipi ((v),BLACKi,WHITEi) )//add clip




#define MIN3(x,y,z)  ((y) <= (z) ? \
                         ((x) <= (y) ? (x) : (y)) \
                     : \
                         ((x) <= (z) ? (x) : (z)))

#define MAX3(x,y,z)  ((y) >= (z) ? \
                         ((x) >= (y) ? (x) : (y)) \
                     : \
                         ((x) >= (z) ? (x) : (z)))



typedef struct  {
    float r, g, b;    /* Channel intensities between 0.0 and 1.0 */
} rgb_color;

typedef struct  {
    int32_t r, g, b;    /* Channel intensities between 0.0 and 1.0 */
} rgb_colorInt32;

typedef struct  {
    float hue;        /* Hue degree between 0.0 and 360.0 */
    float sat;        /* Saturation between 0.0 (gray) and 1.0 */
    float val;        /* Value between 0.0 (black) and 1.0 */
} hsv_color;




COLORSPACE_FASTLOCAL float clipf (const float i, const float imin, const float imax) {
  return std::max(std::min(imax,i),imin);
}

COLORSPACE_FASTLOCAL int clipi (const int i, const int imin, const int imax) {
  return std::max(std::min(imax,i),imin);
}

#define clampI(x) clipi((x),0,0xFFFF)

/*
inline void nullt (float *r, float *g, float *b)
{

}*/

COLORSPACE_FASTLOCAL void rgb2ycbcrf (float *r, float *g, float *b)
{
  float y, cb, cr;
 /* y = 0.2990 * (*r) + 0.5870 * (*g) + 0.1140 * (*b);
  cb = -0.1687 * (*r) - 0.3313 * (*g) + 0.5000 * (*b) + 0.5;
  cr = 0.5000 * (*r) - 0.4187 * (*g) - 0.0813 * (*b) + 0.5;*/
  y  = 0.2990f * (*r) + 0.5870f * (*g) + 0.1140f * (*b);
  cb = ((*b)-y) * 0.564f + 0.5f;
  cr = ((*r)-y) * 0.713f + 0.5f;
  *r = y;
  *g = cb;
  *b = cr;
}

COLORSPACE_FASTLOCAL void ycbcr2rgbf (float *y, float *cb, float *cr)
{

  float r, g, b;
  r = (*y) + 1.40200f * ((*cr) - 0.5f);
  g = (*y) - 0.34414f * ((*cb) - 0.5f) - 0.71414f * ((*cr) - 0.5f);
  b = (*y) + 1.77200f * ((*cb) - 0.5f);
  *y = r;
  *cb = g;
  *cr = b;
}


//============== HSV ===============================================================

COLORSPACE_FASTLOCAL hsv_color RGB2HSV(rgb_color rgb) {
    hsv_color hsv;
    float rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    hsv.val = rgb_max;
    if (hsv.val == 0) {
        hsv.hue = hsv.sat = 0;
        return hsv;
    }

    /* Normalize value to 1 */
    rgb.r /= hsv.val;
    rgb.g /= hsv.val;
    rgb.b /= hsv.val;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    hsv.sat = rgb_max - rgb_min;
    if (hsv.sat == 0) {
        hsv.hue = 0;
        return hsv;
    }

    /* Normalize saturation to 1 */
    rgb.r = (rgb.r - rgb_min)/(rgb_max - rgb_min);
    rgb.g = (rgb.g - rgb_min)/(rgb_max - rgb_min);
    rgb.b = (rgb.b - rgb_min)/(rgb_max - rgb_min);
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    /* Compute hue */
    if (rgb_max == rgb.r) {
        hsv.hue = 0.0f + 60.0f*(rgb.g - rgb.b);
        if (hsv.hue < 0.0f) {
            hsv.hue += 360.0f;
        }
    } else if (rgb_max == rgb.g) {
        hsv.hue = 120.0f + 60.0f*(rgb.b - rgb.r);
    } else /* rgb_max == rgb.b */ {
        hsv.hue = 240.0f + 60.0f*(rgb.r - rgb.g);
    }

    hsv.hue /= (360.f);
    return hsv;

}

COLORSPACE_FASTLOCAL rgb_color HSV2RGB(const hsv_color hsv) {
    rgb_color rgb;
    if(hsv.val == 0.f) {
            rgb.r = 0.f;
            rgb.g = 0.f;
            rgb.b = 0.f;
            return rgb;
        }
    if(hsv.sat == 0.f)  {
            rgb.r = hsv.val;
            rgb.g = hsv.val;
            rgb.b = hsv.val;
            return rgb;
        }
    

    const float hf = hsv.hue * 6;
    const int i    = (int) floor(hf);
    const float f  =  hf - i;
    const float pv = hsv.val*(1-hsv.sat);
    const float qv = hsv.val*(1-hsv.sat*f);
    const float tv = hsv.val*(1-hsv.sat*(1-f));
    
    switch(i){
        case 0: { 
                  rgb.r =hsv.val;
                  rgb.g =tv;
                  rgb.b =pv;} break;
        case 1: {  
                  rgb.r =qv;
                  rgb.g =hsv.val;
                  rgb.b =pv;}break;
        case 2: {
                  rgb.r =pv;
                  rgb.g =hsv.val;
                  rgb.b =tv;}break;
        case 3: {
                  rgb.r =pv;
                  rgb.g =qv;
                  rgb.b =hsv.val;}break;
        case 4: {
                  rgb.r =tv;
                  rgb.g =pv;
                  rgb.b =hsv.val;}break;
        case 5: {
                  rgb.r =hsv.val;
                  rgb.g =pv;
                  rgb.b =qv;}break;
        case 6: { 
                  rgb.r =hsv.val;
                  rgb.g =tv;
                  rgb.b =pv;} break;
        case -1: {
                  rgb.r =hsv.val;
                  rgb.g =pv;
                  rgb.b =qv;}break;
    }          
    
    return rgb;

}

COLORSPACE_FASTLOCAL void hsv2rgb(float *v, float *s, float *h) {
    hsv_color hsv = {*h,*s,*v};
    /*hsl.hue = *h;
    hsl.sat = *s;
    hsl.val = *l;*/
    rgb_color rgb = HSV2RGB(hsv);
    *v = rgb.r;
    *s = rgb.g;
    *h = rgb.b;

}

COLORSPACE_FASTLOCAL void rgb2hsv(float *r, float *g, float *b) {
    rgb_color rgb = {*r,*g,*b};
    hsv_color hsv = RGB2HSV(rgb);
    *r = hsv.val;
    *g = hsv.sat;
    *b = hsv.hue;
}


//=======HCL=========================================================================================
// modified hsl with Y' luminance

COLORSPACE_FASTLOCAL float HCL_VAL(const rgb_color rgb) {
    return 0.299f*rgb.r + .587f*rgb.g + .114f*rgb.b; //ntcs primaries Y'601
//    return 0.2126f*rgb.r + 0.7152f*rgb.g + 0.0722f*rgb.b; //Rec 708
}

COLORSPACE_FASTLOCAL hsv_color RGB2HCL(rgb_color rgb) {
    hsv_color hsv;
    float rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    //hsv.val = rgb_max;
    hsv.val = HCL_VAL(rgb);
    //hsv.val = 0.21f*rgb.r + .72f*rgb.g + .07f*rgb.b; //srgb primries Y'709
    if (hsv.val <= myEpsilon) {
        hsv.hue = hsv.sat = 0.f;
        return hsv;
    }

    /* Normalize value to 1 */
    const float normalize = 1.f/rgb_max;
    rgb.r *= normalize ;
    rgb.g *= normalize;
    rgb.b *= normalize;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    hsv.sat = rgb_max - rgb_min;
    if (hsv.sat <= myEpsilon) {
        hsv.hue = 0.f;
        return hsv;
    }


    /* Normalize saturation to 1 */
    const float normalizeSat = 1.f/(rgb_max - rgb_min);
    rgb.r = (rgb.r - rgb_min)*normalizeSat;
    rgb.g = (rgb.g - rgb_min)*normalizeSat;
    rgb.b = (rgb.b - rgb_min)*normalizeSat;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    /* Compute hue */
    if (rgb_max == rgb.r) {
        hsv.hue = 0.0f + (1.f/6.f)*(rgb.g - rgb.b);
        if (hsv.hue < 0.0f) {
            hsv.hue += 1.f;
        }
    } else if (rgb_max == rgb.g) {
        hsv.hue = (1.f/3.f) + (1.f/6.f)*(rgb.b - rgb.r);
    } else { // rgb_max == rgb.b  
        hsv.hue = (2.f/3.f) + (1.f/6.f)*(rgb.r - rgb.g);
    }
    
    //hsv.hue /= (360.f);
    return hsv;

}

COLORSPACE_FASTLOCAL rgb_color HCL2RGB(const hsv_color hsv) {
    rgb_color rgb;
    if(hsv.val <= myEpsilon) {
            rgb.r = 0.f;
            rgb.g = 0.f;
            rgb.b = 0.f;
            return rgb;
        }
    if(hsv.sat <= myEpsilon)  {
            rgb.r = hsv.val;
            rgb.g = hsv.val;
            rgb.b = hsv.val;
            return rgb;
        }
    

    const float hf = hsv.hue * 6;
    const int i    = (int) floor(hf);
    const float f  = hf - i;
    const float c  = hsv.val*hsv.sat;
    const float qv = c*(1.f-f);
    const float tv = c*f;
    
    switch(i){
        case 0: { 
                  rgb.r =c;
                  rgb.g =tv;
                  rgb.b =0.f;} break;
        case 1: {  
                  rgb.r =qv;
                  rgb.g =c;
                  rgb.b =0.f;}break;
        case 2: {
                  rgb.r =0.f;
                  rgb.g =c;
                  rgb.b =tv;}break;
        case 3: {
                  rgb.r =0.f;
                  rgb.g =qv;
                  rgb.b =c;}break;
        case 4: {
                  rgb.r =tv;
                  rgb.g =0.f;
                  rgb.b =c;}break;
        case 5: {
                  rgb.r =c;
                  rgb.g =0.f;
                  rgb.b =qv;}break;
        case 6: { 
                  rgb.r =c;
                  rgb.g =tv;
                  rgb.b =0.f;} break;
        case -1: {
                  rgb.r =c;
                  rgb.g =0.f;
                  rgb.b =qv;}break;
    }          
    
    //return rgb;

    const float m = hsv.val-HCL_VAL(rgb);
    rgb.r += m;
    rgb.g += m;
    rgb.b += m;
    return rgb;

}




COLORSPACE_FASTLOCAL hsv_color RGB2HSL(rgb_color rgb) {
    hsv_color hsv;
    float rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);


    float rgb_diff;
    float r2, g2, b2;


    hsv.hue = 0.f; // default to black
    hsv.sat = 0.f;
    hsv.val = 0.f;

    hsv.val = (rgb_min + rgb_max) *0.5f;
    if (hsv.val < myEpsilon) {
          return hsv;
    }

    rgb_diff = rgb_max - rgb_min;
    hsv.sat  = rgb_diff;
    if (hsv.sat >= myEpsilon) {
          hsv.sat /= (hsv.val <= 0.5f) ? (rgb_max + rgb_min ) : (2.0f - rgb_max - rgb_min) ;
    } else {
        return hsv;
    }
    r2 = (rgb_max - rgb.r) / rgb_diff;
    g2 = (rgb_max - rgb.g) / rgb_diff;
    b2 = (rgb_max - rgb.b) / rgb_diff;
    if (rgb.r == rgb_max) {
          hsv.hue = (rgb.g == rgb_min ? 5.0f + b2 : 1.0f - g2);
    } else if (rgb.g == rgb_max) {
          hsv.hue = (rgb.b == rgb_min ? 1.0f + r2 : 3.0f - b2);
    } else {
          hsv.hue = (rgb.r == rgb_min ? 3.0f + g2 : 5.0f - r2);
    }
    hsv.hue *= (1.f/6.f);

    return hsv;

}

COLORSPACE_FASTLOCAL rgb_color HSL2RGB(const hsv_color hsv) {
    rgb_color rgb;
    if(hsv.val <= myEpsilon) {
            rgb.r = 0.f;
            rgb.g = 0.f;
            rgb.b = 0.f;
            return rgb;
        }
    if(hsv.sat <= myEpsilon)  {
            rgb.r = hsv.val;
            rgb.g = hsv.val;
            rgb.b = hsv.val;
            return rgb;
        }
    
    float m,v;
    v = (hsv.val <= 0.5f) ? (hsv.val * (1.0f + hsv.sat)) : (hsv.val + hsv.sat - hsv.val * hsv.sat);
    float sv;
    int sextant;
    float fract, vsf, mid1, mid2;

    m = hsv.val + hsv.val - v;
    sv = (v - m ) / v;
    float h = hsv.hue*6.0f;
    sextant = (int)h;
    fract = h - sextant;
    vsf = v * sv * fract;
    mid1 = m + vsf;
    mid2 = v - vsf;
    switch (sextant)  {
        case 6:
        case 0:
              rgb.r = v;
              rgb.g = mid1;
              rgb.b = m;
              break;
        case 1:
              rgb.r = mid2;
              rgb.g = v;
              rgb.b = m;
              break;
        case 2:
              rgb.r = m;
              rgb.g = v;
              rgb.b = mid1;
              break;
        case 3:
              rgb.r = m;
              rgb.g = mid2;
              rgb.b = v;
              break;
        case 4:
              rgb.r = mid1;
              rgb.g = m;
              rgb.b = v;
              break;
        case -1:
        case 5:
              rgb.r = v;
              rgb.g = m;
              rgb.b = mid2;
              break;
    }
    return rgb;

}



COLORSPACE_FASTLOCAL void hsl2rgb(float *l, float *s, float *h) {
    hsv_color hsl = {*h,*s,*l};
    /*hsl.hue = *h;
    hsl.sat = *s;
    hsl.val = *l;*/
    rgb_color rgb = HSL2RGB(hsl);
    *l = rgb.r;
    *s = rgb.g;
    *h = rgb.b;

}

COLORSPACE_FASTLOCAL void rgb2hsl(float *r, float *g, float *b) {
    rgb_color rgb = {*r,*g,*b};
    hsv_color hsl = RGB2HSL(rgb);
    *r = hsl.val;
    *g = hsl.sat;
    *b = hsl.hue;
}

//=======HCL=========================================================================================
// modified hsl with Y' luminance. new version

COLORSPACE_FASTLOCAL hsv_color RGB2HCLnew(rgb_color rgb) {
    hsv_color hsv;
    float rgb_min, rgb_max;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);
    
    //hsv.val = rgb_max;
    hsv.val = HCL_VAL(rgb);
    //hsv.val = 0.21f*rgb.r + .72f*rgb.g + .07f*rgb.b; //srgb primries Y'709
    if (hsv.val <= myEpsilon) {
        hsv.hue = hsv.sat = 0.f;
        return hsv;
    }

    

    hsv.sat = rgb_max - rgb_min;
    if (hsv.sat <= myEpsilon) {
        hsv.hue = 0.f;
        return hsv;
    }
    
    hsv.sat /= hsv.val; //normalize
/* Normalize value to 1 */
    const float normalize = 1.f/rgb_max;
    rgb.r *= normalize ;
    rgb.g *= normalize;
    rgb.b *= normalize;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    /* Normalize saturation to 1 */
    const float normalizeSat = 1.f/(rgb_max - rgb_min);
    rgb.r = (rgb.r - rgb_min)*normalizeSat;
    rgb.g = (rgb.g - rgb_min)*normalizeSat;
    rgb.b = (rgb.b - rgb_min)*normalizeSat;
    rgb_min = MIN3(rgb.r, rgb.g, rgb.b);
    rgb_max = MAX3(rgb.r, rgb.g, rgb.b);

    /* Compute hue */
    if (rgb_max == rgb.r) {
        hsv.hue = 0.0f + (1.f/6.f)*(rgb.g - rgb.b);
        if (hsv.hue < 0.0f) {
            hsv.hue += 1.f;
        }
    } else if (rgb_max == rgb.g) {
        hsv.hue = (1.f/3.f) + (1.f/6.f)*(rgb.b - rgb.r);
    } else { // rgb_max == rgb.b  
        hsv.hue = (2.f/3.f) + (1.f/6.f)*(rgb.r - rgb.g);
    }
    
    //hsv.hue /= (360.f);
    return hsv;

}

COLORSPACE_FASTLOCAL rgb_color HCLnew2RGB(const hsv_color hsv) {
    rgb_color rgb;
    if(hsv.val <= myEpsilon) {
            rgb.r = 0.f;
            rgb.g = 0.f;
            rgb.b = 0.f;
            return rgb;
        }
    if(hsv.sat <= myEpsilon)  {
            rgb.r = hsv.val;
            rgb.g = hsv.val;
            rgb.b = hsv.val;
            return rgb;
        }
    

    const float hf = hsv.hue * 6;
    const int i    = (int) floor(hf);
    const float f  = hf - i;
    const float c  = hsv.val* hsv.sat;
//  const float c  = hsv.sat;
    const float qv = c*(1.f-f);
    const float tv = c*f;
    
    switch(i){
        case 0: { 
                  rgb.r =c;
                  rgb.g =tv;
                  rgb.b =0.f;} break;
        case 1: {  
                  rgb.r =qv;
                  rgb.g =c;
                  rgb.b =0.f;}break;
        case 2: {
                  rgb.r =0.f;
                  rgb.g =c;
                  rgb.b =tv;}break;
        case 3: {
                  rgb.r =0.f;
                  rgb.g =qv;
                  rgb.b =c;}break;
        case 4: {
                  rgb.r =tv;
                  rgb.g =0.f;
                  rgb.b =c;}break;
        case 5: {
                  rgb.r =c;
                  rgb.g =0.f;
                  rgb.b =qv;}break;
        case 6: { 
                  rgb.r =c;
                  rgb.g =tv;
                  rgb.b =0.f;} break;
        case -1: {
                  rgb.r =c;
                  rgb.g =0.f;
                  rgb.b =qv;}break;
    }          
    
    //return rgb;

    const float m = hsv.val-HCL_VAL(rgb);
    rgb.r += m;
    rgb.g += m;
    rgb.b += m;
    return rgb;

}


COLORSPACE_FASTLOCAL void hcl2rgb(float *l, float *c, float *h) {
    hsv_color hcl = {*h,*c,*l};
    rgb_color rgb = HCLnew2RGB(hcl);
    *l = rgb.r;
    *c = rgb.g;
    *h = rgb.b;

}

COLORSPACE_FASTLOCAL void rgb2hcl(float *r, float *g, float *b) {
    rgb_color rgb = {*r,*g,*b};
    hsv_color hcl = RGB2HCLnew(rgb);
    *r = hcl.val;
    *g = hcl.sat;
    *b = hcl.hue;
}







//-------------
COLORSPACE_FASTLOCAL void
rgb2xyz (float *r, float *g, float *b)
{
    //Prophoto D50
    const float x = (0.7976749f  * *r) + (0.1351917f * *g) + (0.0313534f * *b);
    const float y = (0.2880402f  * *r) + (0.7118741f * *g) + (0.0000857f * *b);
    const float z = /*(0.0000000f  * *r) + (0.0000000f * *g) +*/ (0.8252100f * *b);


    *r = x;
    *g = y;
    *b = z;
}

COLORSPACE_FASTLOCAL void
xyz2rgb (float *x, float *y, float *z)
{
    //Prophoto D50
    const float r = (1.3459433f  * *x) + (-0.2556075f * *y) + (-0.0511118f * *z);
    const float g = (-0.5445989f * *x) + (1.5081673f  * *y) + (0.0205351f * *z);
    const float b = /*(0.0000000f  * *x) + (0.0000000f  * *y) +*/ (1.2118128f * *z);

    *x = r;
    *y = g;
    *z = b;
  
}

COLORSPACE_FASTLOCAL void lab2rgb (float *l, float *a, float *b)
{

  float x, y, z;
//http://www.easyrgb.com/index.php?X=MATH&H=08#text8


    /* convert back to normal LAB */
    *l = (*l /*- 0 * 16 * 27 / 24389.0*/) * 116.f;
    *a = (*a - 0.5f) * (500.f * 2.f);
    *b = (*b - 0.5f) * (200.f * 2.2f);

    /* matrix */
    y = (*l + 16.f) *(1.f/ 116.f);
    z = y - *b *(1.f/ 200.0f);
    x = *a *(1.f/ 500.0f) + y;

    /* scale */
    if (x * x * x > (216.f / 24389.f))
      x = x * x * x;
    else
      x = (116.f * x - 16.f) * (27.f / 24389.f);
      
    if (*l > (216.f / 27.f))
      y = y * y * y;
    else
      //y = fimg[0][i] * 27 / 24389.0;
      y = (116.f * y - 16.f) *( 27.f / 24389.f);
      
    if (z * z * z > (216.f / 24389.f))
      z = z * z * z;
    else
      z = (116.f * z - 16.f) * (27.f / 24389.f);

    /* white reference */
    *l = x * 0.95047f;
    *a = y;
    *b = z * 1.08883f;

  xyz2rgb(l,a,b);
}



COLORSPACE_FASTLOCAL void rgb2lab (float *r, float *g, float *b)
{
  float ll, aa, bb;
  rgb2xyz(r,g,b);

    /* reference white */
    *r *= (1.f/0.95047f);
    /* (just for completeness)
    *g /= 1.00000; */
    *b *= (1.f/1.08883f);

    /* scale */
    if (*r > (216.f / 24389.0f)) {
      *r = fastcbrt(*r);//powf(*r, (1 / 3.0f));
    } else {
      *r = (24389.f * *r / 27.0f + 16.f) / 116.0f;
    }
    if (*g > (216.f / 24389.0f)) {
      *g = fastcbrt(*g);//powf(*g, (1 / 3.0f));
    } else {
      *g = (24389.f * *g / 27.0f + 16.f) / 116.0f;
    }
    if (*b > (216.f / 24389.0f)) {
      *b = fastcbrt(*b);//powf(*b, (1 / 3.0f));
    } else {
      *b = (24389.f * *b / 27.0f + 16.f) / 116.0f;
    }

    //ll = 116.f * *g - 16.f;
    
    if (*g > 0.008856f) {
            ll = (116.f * *g) - 16.f;
        } else {
            ll = 903.3f * *g;
        }
    
    aa = 500.f * (*r - *g);
    bb = 200.f * (*g - *b);
    *r = ll / 116.0f; // + 16 * 27 / 24389.0;
    *g = aa / 500.0f / 2.0f + 0.5f;
    *b = bb / 200.0f / 2.2f + 0.5f;
    if (*r < 0.f)
      *r = 0.f;
  
}

#endif
