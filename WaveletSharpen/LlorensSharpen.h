#ifndef LLORENSSHARPEN_H
#define LLORENSSHARPEN_H
#include "colorspace.h"


//void edgeMask(float *fimg, float *fimgG,const unsigned int width, const unsigned int height, const int passes);
void LlorensSharpen(float *fimg, const unsigned int width, const unsigned int height,const int passes, const float strength);
void microcontrast(float *fimg, const unsigned int width, const unsigned int height,float strength);

#endif //LLORENSSHARPEN_H


