/*
 * (C) 2010, Roland Baudin <roland65@free.fr>
 * The implemented IIR filter is a very fast and very accurate approximation of a gaussian filter.
 * The theory of this filter is adapted from the following papers :
 * "Recursive implementation of the Gauxssian filter", by I.T. Young and L.J. Van Vliet, Signal Processing, 1995, pp. 139-151
 * "Recursive Gaussian Derivative Filters" by L.J. van Vliet, I.T. Young, and P.W. Verbeek, IPRC'98, Vol 1, pp. 509-514
 * The papers can be downloaded here :
 * <http://homepage.tudelft.nl/e3q6n/publications/1995/SP95TYLV/SP95TYLV.html>
 * <http://homepage.tudelft.nl/e3q6n/publications/1998/ICPR98LVTYPV/ICPR98LVTYPV.html>
 *
 * Other authors :
 * (C) 2010 Dieter Steiner <spoilerhead@gmail.com> (Bibble Wavelet Sharpen plugin)
 * (C) 1999 Winston Chang <winstonc@cs.wisc.edu> and Peter Heckert (Gimp IIR plugin)
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

#include "gauss_iir.h"
#include <math.h>
#include <algorithm>
#include <cmath>

//#include <stdio.h>


// IIR filter version
//#define IIR95_3      // Van Vliet 1995 - 3 coefs
#define IIR98_3      // Van Vliet 1998 - 3 coefs
//#define IIR98_5      // Van Vliet 1998 - 5 coefs


using namespace std;

inline float clip(const float x)
{
    return std::max(std::min( x , 100.0f),0.0f);
}



#ifdef IIR95_3
typedef struct
{
    float B, b1, b2, b3, r, q;
} iir_param;


static void iir_init(iir_param *iir, const float r)
{

    iir->r = r;
    float b0, q;

    if ( r >= 2.5) q = 0.98711 * r - 0.96330;
    else q = 3.97156 - 4.14554 * sqrt(1.0-0.26891 * r);

    iir->q = q;
    b0 = 1.57825 + ((0.422205 * q  + 1.4281) * q + 2.44413) *  q;
    iir->b1 = ((1.26661 * q +2.85619) * q + 2.44413) * q / b0;
    iir->b2 = - ((1.26661*q +1.4281) * q * q ) / b0;
    iir->b3 = 0.422205 * q * q * q / b0;
    iir->B = 1.0 - (iir->b1 + iir->b2 + iir->b3);
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
 * float floating point precision is necessary for radius > 50, as my experiments
 * have shown. On my system (Duron, 1,2 GHz) the speed difference between float
 * and float is neglectable.
 */
{
    float *const lp = data, *const rp = data + ((width-1)*nextPixel);

    /* Forward filtering */
    {
        /* Hoping compiler will use optimal alternative, if not enough registers */
        register float d1,d2,d3;
        data = lp;    
        d1=d2=d3=*data;
        while (data <=  rp)
        {
            *data *=  iir->B;
			*data +=  iir->b3 * d3;
			*data +=  iir->b2 * (d3 = d2);
			*data +=  iir->b1 * (d2 = d1);
			d1 = *data;
            *data = clip(*data);
            data += nextPixel;
        }
    }

    /* Backward filtering */
    data -=nextPixel;
    {
        /* Hoping compiler will use optimal alternative, if not enough registers */
        register float d1,d2,d3;
        d1=d2=d3=*data;
        while (data >=  lp)
        {
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

#endif




#ifdef IIR98_3
typedef struct
{
    float a, b1, b2, b3, r, q;
} iir_param;


/* Compute IIR filter coefficients */
static void iir_init(iir_param *iir, const float r)
{
    // Radius
    iir->r = r;

    /* Scaling parameter, based on the radius value */
    float q;

	if ( r >= 4.0f)
    	q = 0.4713f*r + 0.0309f;
    else if (r >= 1.0f)
    	q = 0.4469f*r + 0.1286f;
    else
		q = 0.3138f + r*(0.0382f + r*(0.3296f -r*0.1062f));

	iir->q = q;

	/* Filter coefficients */
    float b, a1, a2, a3;

	a1 = pow(3.0231209741f, 1.0f/q);
	a2 = pow(1.7387124472f, 1.0f/q) * cos(0.61861345871f/q);
	a3 = pow(1.8654300000f, 1.0f/q);
	b = 1.0f/(a1*a3);

	iir->b3 = b;
	iir->b2 = -b*(2.0f*a2 +a3);
	iir->b1 = b*(a1 + 2.0f*a2*a3);
	iir->a = 1.0f - (iir->b1 + iir->b2 + iir->b3);
}


/* Filter a vector of size width */
static void iir_filter(iir_param *iir, float *data, const int width, const int nextPixel)
/*
 * Very fast gaussian blur with infinite impulse response filter
 * The row is blurred in forward direction and then in backward direction
 * So we achieve zero phase errors and symmetric impulse response
 * and good isotropy
 *
 * Theory for this filter can be found at:
 * <http://homepage.tudelft.nl/e3q6n/publications/1998/ICPR98LVTYPV/ICPR98LVTYPV.html>
 * It is usable for radius downto 0.1.
 */
{
    float *const lp = data, *const rp = data + ((width-1)*nextPixel);

    /* Forward filtering */
    {
        /* Hoping compiler will use optimal alternative, if not enough registers */
        register float d1, d2, d3;
        data = lp;
   		d1=d2=d3=*data;

        while (data <=  rp)
        {
            *data *=  iir->a;
            *data +=  iir->b3 * d3;
            *data +=  iir->b2 * (d3 = d2);
            *data +=  iir->b1 * (d2 = d1);
            d1 = *data;
            *data = clip(*data);
    		    
            data += nextPixel;
        }
    }

    /* Backward filtering */
    data -= nextPixel;
	{
        /* Hoping compiler will use optimal alternative, if not enough registers */
        register float d1, d2, d3;
        d1=d2=d3=*data;
        while (data >=  lp)
        {
            *data *=  iir->a;
            *data +=  iir->b3 * d3;
            *data +=  iir->b2 * (d3 = d2);
            *data +=  iir->b1 * (d2 = d1);
            d1 = *data;
            *data = clip(*data);

            data -= nextPixel;
        }
    }
}

#endif



#ifdef IIR98_5
typedef struct
{
    float a, b1, b2, b3, b4, b5, r, q;
} iir_param;


/* Compute IIR filter coefficients */
static void iir_init(iir_param *iir, const float r)
{
    /* Radius */
    iir->r = r;

    /* Scaling parameter q, based on the radius value r */
    float q;
    if ( r >= 4.0)
        q = 0.4523 * r + 0.0549;
    else
        q = 0.4164 + r*(0.0090 + r*(0.2691 + r*(-0.0920 + r*(0.0164 - r*0.0012))));
    iir->q = q;

    /* Filter coefficients */
    float b, a1, a2, a3, a4, a5, a6, a7;

    a1 = pow(2.8608106221, 1.0/q);
    a2 = pow(3.2971875445, 1.0/q);
    a3 = pow(1.8750400000, 1.0/q);
    a4 = pow(1.691393100996927, 1.0/q) * cos(1.034449869750716/q);
    a5 = pow(1.815815944554954, 1.0/q) * cos(0.475555913067523/q);
    a6 = pow(3.071258561300467, 1.0/q) * cos(1.510005782818239/q);
    a7 = pow(3.071258561300467, 1.0/q) * cos(0.558893956683194/q);
    b = 1.0/(a1*a2*a3);

    iir->b5 = b;
    iir->b4 = -b*(2.0*a4 + 2.0*a5 + a3);
    iir->b3 = b*(a1 + a2 + 2.0*a7 + 2.0*a6 + 2.0*a3*a4 + 2.0*a3*a5);
    iir->b2 = -b*(2.0*a1*a5 + 2.0*a2*a4 + a1*a3 + a2*a3 + 2.0*a3*a6 + 2.0*a3*a7);
    iir->b1 = b*(a1*a2 + 2.0*a1*a3*a5 + 2.0*a2*a3*a4);
    iir->a = 1.0 - (iir->b1 + iir->b2 + iir->b3 + iir->b4 + iir->b5);
}


/* Filter a vector of size width */
static void iir_filter(iir_param *iir, float *data, const int width, const int nextPixel)
/*
 * Very fast gaussian blur with infinite impulse response filter
 * The row is blurred in forward direction and then in backward direction
 * So we achieve zero phase errors and symmetric impulse response
 * and good isotropy
 *
 * Theory for this filter can be found at:
 * <http://homepage.tudelft.nl/e3q6n/publications/1998/ICPR98LVTYPV/ICPR98LVTYPV.html>
 * It is usable for radius downto 0.1.
 */
{
    float *const lp = data, *const rp = data + ((width-1)*nextPixel);

    /* Forward filtering */
    {
        /* Hoping compiler will use optimal alternative, if not enough registers */
        register float d1, d2, d3, d4, d5;
        data = lp;
   		d1=d2=d3=d4=d5=*data;

        while (data <=  rp)
        {
            *data *=  iir->a;
            *data +=  iir->b5 * d5;
            *data +=  iir->b4 * (d5 = d4);
            *data +=  iir->b3 * (d4 = d3);
            *data +=  iir->b2 * (d3 = d2);
            *data +=  iir->b1 * (d2 = d1);
            d1 = *data;
            *data = clip(*data);
    		    
            data += nextPixel;
        }
    }

    /* Backward filtering */
    data -= nextPixel;
   {
        /* Hoping compiler will use optimal alternative, if not enough registers */
        register float d1, d2, d3, d4, d5;
        d1=d2=d3=d4=d5=*data;
        while (data >=  lp)
        {
            *data *=  iir->a;
            *data +=  iir->b5 * d5;
            *data +=  iir->b4 * (d5 = d4);
            *data +=  iir->b3 * (d4 = d3);
            *data +=  iir->b2 * (d3 = d2);
            *data +=  iir->b1 * (d2 = d1);
            d1 = *data;
            *data = clip(*data);

            data -= nextPixel;
        }
    }
}

#endif


void GAUSS_IIR(float *fimg, const unsigned int width, const unsigned int height, const float radius)
{
    if (radius == 0)
        return;

    unsigned int col, row;
    iir_param iir;

    // Get filter parameters
    iir_init(&iir,radius);

    // Blur row
    for (row = 0; row < height; row++)
    {
        iir_filter(&iir, fimg+(row*width), width, 1);
    }
    // Blur col
    for (col = 0; col < width; col++)
    {
        iir_filter(&iir, fimg+col, height, width);
    }
}
