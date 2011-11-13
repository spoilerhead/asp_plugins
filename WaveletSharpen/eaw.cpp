/*
    This file is part of darktable,
    copyright (c) 2009--2010 johannes hanika.

    darktable is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    darktable is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with darktable.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <cmath>
#include <cstring>

#include "eaw.h"

/* local function prototypes */
inline float EAW_CLAMP( float x, float a, float b) {
    return x < a ? a : (x > b ? b : x);
};
inline float EAW_MIN( float x, float y) {
    return x < y ? x : y;
};
// edge-avoiding wavelet:
#define gweight(i, j, ii, jj) 1.0/(fabsf(weight_a[l][wd*((j)>>(l-1)) + ((i)>>(l-1))] - weight_a[l][wd*((jj)>>(l-1)) + ((ii)>>(l-1))])+1.e-5)
// #define gweight(i, j, ii, jj) 1.0/(powf(fabsf(weight_a[l][wd*((j)>>(l-1)) + ((i)>>(l-1))] - weight_a[l][wd*((jj)>>(l-1)) + ((ii)>>(l-1))]),0.8)+1.e-5)
// std cdf(2,2) wavelet:
// #define gweight(i, j, ii, jj) (wd ? 1.0 : 1.0) //1.0
/*
 * For 3 channels:
 * #define gbuf(BUF, A, B) ((BUF)[3*width*((B)) + 3*((A)) + ch])
 */
#define gbuf(BUF, A, B) ((BUF)[width*((B)) + ((A))])


void dt_iop_equalizer_wtf(float *buf, float **weight_a, const int l, const int width, const int height)
{
    const int wd = (int)(1 + (width>>(l-1))), ht = (int)(1 + (height>>(l-1)));
    int ch = 0;
    // store weights for luma channel only, chroma uses same basis.
    memset(weight_a[l], 0, sizeof(float)*wd*ht);
    for (int j=0; j<ht-1; j++) for (int i=0; i<wd-1; i++) weight_a[l][j*wd+i] = gbuf(buf, i<<(l-1), j<<(l-1));

    const int step = 1<<l;
    const int st = step/2;

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(weight_a,buf) private(ch) schedule(static)
#endif
    for (int j=0; j<height; j++)
    { // rows
        // precompute weights:
        float *tmp = new float[width];
        for (int i=0; i<width-st; i+=st) tmp[i] = gweight(i, j, i+st, j);
        // predict, get detail
        int i = st;
        for (; i<width-st; i+=step) /*for (ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) -= (tmp[i-st]*gbuf(buf, i-st, j) + tmp[i]*gbuf(buf, i+st, j))
                                   /(tmp[i-st] + tmp[i]);
        if (i < width) /*for (ch=0; ch<3; ch++)*/ gbuf(buf, i, j) -= gbuf(buf, i-st, j);
        // update coarse
/*        for (ch=0; ch<3; ch++)*/ gbuf(buf, 0, j) += gbuf(buf, st, j)*0.5f;
        for (i=step; i<width-st; i+=step) /*for (ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) += (tmp[i-st]*gbuf(buf, i-st, j) + tmp[i]*gbuf(buf, i+st, j))
                                   /(2.0*(tmp[i-st] + tmp[i]));
        if (i < width) /*for (ch=0; ch<3; ch++)*/ gbuf(buf, i, j) += gbuf(buf, i-st, j)*.5f;
		delete[] tmp;
    }
#ifdef _OPENMP
#pragma omp parallel for default(none) shared(weight_a,buf) private(ch) schedule(static)
#endif
    for (int i=0; i<width; i++)
    { // cols
        // precompute weights:
        float * tmp = new float[height];
        for (int j=0; j<height-st; j+=st) tmp[j] = gweight(i, j, i, j+st);
        int j = st;
        // predict, get detail
        for (; j<height-st; j+=step) /*for (ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) -= (tmp[j-st]*gbuf(buf, i, j-st) + tmp[j]*gbuf(buf, i, j+st))
                                   /(tmp[j-st] + tmp[j]);
        if (j < height) /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, j) -= gbuf(buf, i, j-st);
        // update
        /*for (ch=0; ch<3; ch++) */gbuf(buf, i, 0) += gbuf(buf, i, st)*0.5;
        for (j=step; j<height-st; j+=step) /*for (ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) += (tmp[j-st]*gbuf(buf, i, j-st) + tmp[j]*gbuf(buf, i, j+st))
                                   /(2.0*(tmp[j-st] + tmp[j]));
        if (j < height) /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, j) += gbuf(buf, i, j-st)*.5f;
		delete[] tmp;
    }
}

void dt_iop_equalizer_iwtf(float *buf, float **weight_a, const int l, const int width, const int height)
{
    const int step = 1<<l;
    const int st = step/2;
    const int wd = (int)(1 + (width>>(l-1)));

#ifdef _OPENMP
#pragma omp parallel for default(none) shared(weight_a,buf) schedule(static)
#endif
    for (int i=0; i<width; i++)
    { //cols
       float * tmp = new float[height];;
        int j;
        for (j=0; j<height-st; j+=st) tmp[j] = gweight(i, j, i, j+st);
        // update coarse
        /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, 0) -= gbuf(buf, i, st)*0.5f;
        for (j=step; j<height-st; j+=step) /*for (int ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) -= (tmp[j-st]*gbuf(buf, i, j-st) + tmp[j]*gbuf(buf, i, j+st))
                                   /(2.0*(tmp[j-st] + tmp[j]));
        if (j < height) /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, j) -= gbuf(buf, i, j-st)*.5f;
        // predict
        for (j=st; j<height-st; j+=step) /*for (int ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) += (tmp[j-st]*gbuf(buf, i, j-st) + tmp[j]*gbuf(buf, i, j+st))
                                   /(tmp[j-st] + tmp[j]);
        if (j < height) /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, j) += gbuf(buf, i, j-st);
		delete[] tmp;
    }
#ifdef _OPENMP
#pragma omp parallel for default(none) shared(weight_a,buf) schedule(static)
#endif
    for (int j=0; j<height; j++)
    { // rows
        float *tmp = new float [width];
        int i;
        for (int i=0; i<width-st; i+=st) tmp[i] = gweight(i, j, i+st, j);
        // update
        /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, 0, j) -= gbuf(buf, st, j)*0.5f;
        for (i=step; i<width-st; i+=step) /*for (int ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) -= (tmp[i-st]*gbuf(buf, i-st, j) + tmp[i]*gbuf(buf, i+st, j))
                                   /(2.0*(tmp[i-st] + tmp[i]));
        if (i < width) /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, j) -= gbuf(buf, i-st, j)*0.5f;
        // predict
        for (i=st; i<width-st; i+=step) /*for (int ch=0; ch<3; ch++)*/
                gbuf(buf, i, j) += (tmp[i-st]*gbuf(buf, i-st, j) + tmp[i]*gbuf(buf, i+st, j))
                                   /(tmp[i-st] + tmp[i]);
        if (i < width) /*for (int ch=0; ch<3; ch++)*/ gbuf(buf, i, j) += gbuf(buf, i-st, j);
		delete[] tmp;
    }
}

#undef gbuf
#undef gweight


void
EAW_process (float *image,
         int    width,
         int    height,
         float  alpha,
         int    maxband,
         int    inband,
         int    mode)
{
    /* PROCESSING*/
    // 1 pixel in this buffer represents 1.0/scale pixels in original image:
    const float l1 = 1.0f; // finest level
    float lm = 0;
    for (int k = EAW_MIN(width,height); k; k>>=1) lm++; // coarsest level
    lm = EAW_MIN(maxband, l1 + lm);
    // level 1 => full resolution

    int numl = 0;
    for (int k=EAW_MIN(width,height); k; k>>=1) numl++;
    const int numl_cap = EAW_MIN(maxband - l1 + 1.5, numl);
    // printf("level range in %d %d: %f %f, cap: %d\n", 1, maxband, l1, lm, numl_cap);

    // TODO: fixed alloc for data piece at capped resolution?
    float **tmp = (float **)malloc(sizeof(float *)*numl_cap);
    for (int k=1; k<numl_cap; k++)
    {
        const int wd = (int)(1 + (width>>(k-1)));
        const int ht = (int)(1 + (height>>(k-1)));
        tmp[k] = (float *)malloc(sizeof(float)*wd*ht);
    }

    for (int level=1; level<numl_cap; level++) dt_iop_equalizer_wtf(image, tmp, level, width, height);

    for (int l=1; l<numl_cap; l++)
    {
        const float lv = (lm-l1)*(l-1)/(float)(numl_cap-1) + l1; // appr level in real image.
        const float band = EAW_CLAMP((1.0 - lv / maxband), 0, 1.0);
        // printf("lv: %f; band: %f, step: %d\n", lv, band, 1<<l);

        // coefficients in range [0, 2], 1 being neutral.
        float coeff = 1;
        if (mode == 0) { // linear amplification
            if(alpha>1.0) coeff = 1 + fabs(1-alpha)/lv;
            else          coeff = 1 - fabs(1-alpha)/lv;
        } else if (mode == 1) { // suppress lowest level
            if      ((int)lv == 1) coeff = 1;
            else if ((int)lv == 2) coeff = (alpha>1?(fabs(1-alpha)/lv/2+1):(1-fabs(1-alpha)/lv/2));
            else                   coeff = (alpha>1?(fabs(1-alpha)/lv+1):(1-fabs(1-alpha)/lv));
        } else if (mode == 2) { // show only one level
            if ((int)lv == inband ) coeff = alpha;
            else coeff = 0;
        }

        //for (int ch=0; ch<3; ch++)
        {
            const int step = 1<<l;
#if 1 // scale coefficients
            for (int j=0; j<height; j+=step)      for (int i=step/2; i<width; i+=step) image[width*j + i ] *= coeff;
            for (int j=step/2; j<height; j+=step) for (int i=0; i<width; i+=step)      image[width*j + i ] *= coeff;
            for (int j=step/2; j<height; j+=step) for (int i=step/2; i<width; i+=step) image[width*j + i ] *= coeff*coeff;
#else // soft-thresholding (shrinkage)
#define wshrink (copysignf(fmaxf(0.0f, fabsf(image[width*j + i]) - (1.0-coeff)), image[width*j + i ]))
            for (int j=0; j<height; j+=step)      for (int i=step/2; i<width; i+=step) image[width*j + i ] = wshrink;
            for (int j=step/2; j<height; j+=step) for (int i=0; i<width; i+=step)      image[width*j + i ] = wshrink;
            for (int j=step/2; j<height; j+=step) for (int i=step/2; i<width; i+=step) image[width*j + i ] = wshrink;
#undef wshrink
#endif
        }
    }
    // printf("applied\n");
    for (int level=numl_cap-1; level>0; level--) dt_iop_equalizer_iwtf(image, tmp, level, width, height);

    for (int k=1; k<numl_cap; k++) free(tmp[k]);
    free(tmp);
}
