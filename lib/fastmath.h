#ifndef FASTMATH_H
#define FASTMATH_H

#include <cmath>

#define fastcbrt halley_cbrt2f



#define FASTMATH_LOCAL static inline

#define myEpsilon 0.000001f
#ifndef M_PI
    #define M_PI    3.14159265358979323846f
#endif

// =========================== CUBIC ROOT =====================================
// cube root approximation using bit hack for 32-bit float
FASTMATH_LOCAL float cbrt_5f(float f)
{
	unsigned int* p = (unsigned int *) &f;
	*p = *p/3 + 709921077;
	return f;
}

// iterative cube root approximation using Halley's method (float)
FASTMATH_LOCAL float cbrta_halleyf(const float a, const float R)
{
	const float a3 = a*a*a;
    const float b= a * (a3 + R + R) / (a3 + a3 + R);
	return b;
}

// cube root approximation using 2 iterations of Halley's method (float) ~20 bits
FASTMATH_LOCAL float halley_cbrt2f(float d)
{
	float a = cbrt_5f(d);
	a = cbrta_halleyf(a, d);
	return cbrta_halleyf(a, d);
}

// cube root approximation using 1 iterations of Halley's method (float) ~15bit
FASTMATH_LOCAL float halley_cbrt1f(float d)
{
	float a = cbrt_5f(d);
	return cbrta_halleyf(a, d);
}

float FASTMATH_LOCAL fastsin(float x) {
    //always wrap input angle to -PI..PI
    if (x < -M_PI)
        x += 2.f*M_PI;
    else if (x >  M_PI)
        x -= 2.f*M_PI;

    //compute sine
    if (x < 0.f)
        return (1.27323954f + 0.405284735f * x) * x;
    else
        return (1.27323954f - 0.405284735f * x) * x;
}

float FASTMATH_LOCAL fastcos(float x) {
    x += M_PI/2.f;
    return fastsin(x);
}

//looks good, but the error is a bit high
float FASTMATH_LOCAL fastatan2(const float y, const float x)
{
   float  angle;
   const float coeff_1 = M_PI/4.f;
   const float coeff_2 = 3.f*coeff_1;
   float abs_y = fabsf(y)+1e-10f;      // kludge to prevent 0/0 condition
   if (x>=0.f)
   {
      float r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      float r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }


   if (y < 0.f)
           return(-angle);     // negate if in quad III or IV
   else
           return(angle);
}


float FASTMATH_LOCAL fastsqrt( const float fg)
{ 
    float n = fg / 2.0f;
    float lstX = 0.0f; 
    while(n != lstX) { 
        lstX = n;
        n = (n + fg/n) *.5f;/// 2.0f; 
    }
    return n;
}

float FASTMATH_LOCAL fastsqrt2(const float x)  
{
  union
  {
    int i;
    float x;
  } u;
  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22); 
  
  // Two Babylonian Steps (simplified from:)
  // u.x = 0.5f * (u.x + x/u.x);
  // u.x = 0.5f * (u.x + x/u.x);
  u.x =       u.x + x/u.x;
  u.x = 0.25f*u.x + x/u.x;

  return u.x;
}

#endif //FASTMATH_H
