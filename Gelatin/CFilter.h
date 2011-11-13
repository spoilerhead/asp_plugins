#ifndef CFILTER_H
#define CFILTER_H
#include <cmath>
#include <algorithm>

//just to be sure and avoid compiling errors on funny compilers, not needed on gcc
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#define SPECTRAL_POINTS 18


typedef struct  {
    float spectral[SPECTRAL_POINTS];
    float colorShiftA[2]; //(dark,bright)
    float colorShiftB[2];
    char name[40];//std::string name;
} filterType;

typedef struct  {
    float points[10];
    float max;  //max value
    float base; //base fog
    float expMult; //exposure multiplier
    char name[64];//std::string name;

} contrastType;

typedef struct  {
    float L;
    float A;
    float B;
} pixelLABf;



class CFilter {
    public:
    CFilter(const filterType  pfilter) {
        filter = pfilter;
        //Compute filter Average Value
    	Avg = 0.f;
    	float sumFilter = 0.f; //sum of all filter coefficients
	    for(int i = 0; i<SPECTRAL_POINTS; i++) { sumFilter += filter.spectral[i]; }
	    Avg = sumFilter/SPECTRAL_POINTS;
	    invAvg = 1.f/Avg;
	    //for(int i = 0; i<SPECTRAL_POINTS; i++) { Avg = (filter.spectral[i]>Avg)?filter.spectral[i]:Avg; } //maximum
	    
	    
	    //compute filter base tint
	    TintA =0.f; TintB =0.f;
	    for(int i = 0; i<SPECTRAL_POINTS;i++) { 
            const float phi = (i*2.f*M_PI/SPECTRAL_POINTS)+M_PI; //bring angle into the 2PI range
            const float v   = filter.spectral[i]; //Value
	        TintA += v*cos(phi);
	        TintB += v*sin(phi);
	    }
	    //Normalize
	    TintA /= sumFilter;
	    TintB /= sumFilter;
	    
	    extremeTint = false;
	    if( (abs(TintA) > 1.f) || (abs(TintB) >1.f)) {
	        float maxTint = std::max(abs(TintA),abs(TintB));
	        TintA /=(maxTint);
	        TintB /=(maxTint);
	        extremeTint = true;
	    }

  
    };
    filterType filter;
    float Avg;
    float invAvg; //inverse average, i.e. 1/Avg
    float TintA;
    float TintB;
    bool extremeTint; //do we have an extreme tint overshadowing everything else?
};

#endif //CFILTER_H
