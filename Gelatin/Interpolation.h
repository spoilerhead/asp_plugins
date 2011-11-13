#ifndef INTERPOLATION_H
#define INTERPOLATION_H
#include <cmath>
//http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/interpolation/
//def as '' for debug purpose
#define INLINE inline
INLINE float LinearInterpolate(
   const float y1,const float y2,
   const float mu)
{
   return(y1*(1.f-mu)+y2*mu);
}

INLINE float CosineInterpolate(
   const float y1,const float y2,
   const float mu)
{
   float mu2;

   mu2 = (1.f-cos(mu*M_PI))/2.f;
   return(y1*(1.f-mu2)+y2*mu2);
}

INLINE float CubicInterpolate(
   const float y0,const float y1,
   const float y2,const float y3,
   const float mu)
{
   float a0,a1,a2,a3,mu2;

   mu2 = mu*mu;
   a0 = y3 - y2 - y0 + y1;
   a1 = y0 - y1 - a0;
   a2 = y2 - y0;
   a3 = y1;

   return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

INLINE float CatmullRomInterpolate(
   const float y0,const float y1,
   const float y2,const float y3,
   const float mu)
{
   float a0,a1,a2,a3,mu2;
   mu2 = mu*mu;
   a0 = -0.5f*y0 + 1.5f*y1 - 1.5f*y2 + 0.5f*y3;
   a1 = y0 - 2.5f*y1 + 2.f*y2 - 0.5f*y3;
   a2 = -0.5f*y0 + 0.5f*y2;
   a3 = y1;

   return(a0*mu*mu2+a1*mu2+a2*mu+a3);
}

/*
   Tension: 1 is high, 0 normal, -1 is low
   Bias: 0 is even,
         positive is towards first segment,
         negative towards the other
*/
INLINE float HermiteInterpolate(
   const float y0,const float y1,
   const float y2,const float y3,
   const float mu,
   const float tension,
   const float bias)
{
   float m0,m1,mu2,mu3;
   float a0,a1,a2,a3;

	mu2 = mu * mu;
	mu3 = mu2 * mu;
    m0  = (y1-y0)*(1.f+bias)*(1.f-tension)/2.f;
    m0 += (y2-y1)*(1.f-bias)*(1.f-tension)/2.f;
    m1  = (y2-y1)*(1.f+bias)*(1.f-tension)/2.f;
    m1 += (y3-y2)*(1.f-bias)*(1.f-tension)/2.f;
    a0 =  2.f*mu3 - 3.f*mu2 + 1.f;
    a1 =    mu3 - 2.f*mu2 + mu;
    a2 =    mu3 -   mu2;
    a3 = -2.f*mu3 + 3.f*mu2;

   return(a0*y1+a1*m0+a2*m1+a3*y2);
}


#endif //INTERPOLATION_H
