#ifndef _WAVELET_H
#define _WAVELET_H

#include <string>
#include <cmath>

using namespace std;

void wavelet_denoise (float *fimg[3], const unsigned int width, const unsigned int height,
                 const float threshold, const float low);
                 

#endif
