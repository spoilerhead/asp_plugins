/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
#ifndef _USM_IIR_H
#define _USM_IIR_H

// Buffer do channel, width of buffer, height of buffer, radius of the filter
void USM_IIR (float *fimg, const unsigned int width, const unsigned int height,const float radius);
void USM_IIR_stacked (float *fimg, const unsigned int width, const unsigned int height,const float radius, const float stackRadius);
                 
#endif
