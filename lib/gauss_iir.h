/*
 * (C) 2010 Roland Baudin <roland65@free.fr> and Dieter Steiner <spoilerhead@gmail.com>
 * Licensed: GPL v2+
 */
#ifndef _GAUSS_IIR_H
#define _GAUSS_IIR_H

// Buffer do channel, width of buffer, height of buffer, radius of the filter
void GAUSS_IIR(float *fimg, const unsigned int width, const unsigned int height, const float radius);

#endif
