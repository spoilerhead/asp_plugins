#ifndef _WAVELET_H
#define _WAVELET_H

#include <string>
#include <cmath>

using namespace std;

void wavelet_sharpenF (float *fimg[3], const unsigned int width, const unsigned int height,
                 const float amount, const float radius);

/*
void wavelet_sharpenI (int *fimg[3], const unsigned int width, const unsigned int height,
                 const float amount, const float radius);                 */
#endif
