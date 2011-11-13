//TODO check http://photivo.org/photivo/start?redirect=1
//====================================================0
// To the extent possible under law, Manuel Llorens <manuelllorens@gmail.com>
// has waived all copyright and related or neighboring rights to this work.
// This code is licensed under CC0 v1.0, see license information at
// http://creativecommons.org/publicdomain/zero/1.0/
//http://www.rawness.es/sharpening/?lang=en


#include "LlorensSharpen.h"
#include <QDebug>

//opt
/*
inline float fastsqrt(float val)  {
        union
        {
                int tmp;
                float val;
        } u;
        u.val = val;
        u.tmp = (1<<29) + (u.tmp >> 1) - (1<<22) + -0x4C000;
        return u.val;
}*/

#include "convolution.h"
#include "usm_iir.h"

#define SPH_VERSION

//static float sharpen[5] = {-1.f/4.f, -1.f/4.f, 2.f, -1.f/4.f, -1.f/4.f};



static inline float hardT(const float value,const float t) { 
    return (value>=t)?1.f:0.f;
}




#pragma intrinsic(fabs)
#define FABS fabs
#define SQRT sqrt

#ifdef SPH_VERSION  //adaptive gradient version sph
//#define SHOWMASK    //show the gradient mask

//#define USESCHARR //define this to use the scharr operator instead of the normal gradient operator http://en.wikipedia.org/wiki/Sobel_operator
#define USEHoloborodko //elseif use http://www.holoborodko.com/pavel/?page_id=1660 (has nice noise reduction properties

static void edgeMask(float *fimg, float *fimgG,const unsigned int width, const unsigned int height, const int passes) {
    #ifdef USESCHARR
    float gradient[3] = {3.f/16.f,10.f/16.f,3.f/16.f};
    float gradt[3] = {1.f,0.f,-1.f};
    #elif defined USEHoloborodko
    float gradient[5] = {-1.f, -2.f, 0.f, 2.f, 1.f};
    float gradt[3] = {.25f,0.5f,.25f};
    #else
    float gradient[3] = {-0.5f,0.f,0.5f};
    float identity[1] = {1.f};
    #endif

    float *gradX = new float[width*height];
    float *gradY = new float[width*height];
    
    memcpy(fimgG,fimg,width*height*sizeof(float));
    
    USM_IIR(fimgG,width,height,1.0f+passes/4.f);
    #ifdef   USESCHARR
    convolve2DSeparable(fimgG,gradX,width,height,gradient,3,gradt,3);
    convolve2DSeparable(fimgG,gradY,width,height,gradt,3,gradient,3);
    const float gradTres = 0.0050f-(passes-2.5f)/10000.f;
    #elif defined USEHoloborodko
    convolve2DSeparable(fimgG,gradX,width,height,gradient,5,gradt,3);
    convolve2DSeparable(fimgG,gradY,width,height,gradt,3,gradient,5);
    const float gradTres = 0.0200f-(passes-2.5f)/10000.f;
    #else
    convolve2DSeparable(fimgG,gradX,width,height,gradient,3,identity,1);
    convolve2DSeparable(fimgG,gradY,width,height,identity,1,gradient,3);
    const float gradTres = 0.0025f-(passes-2.5f)/10000.f;
    #endif
    
    for(int i = 0; i<(width*height);i++) {
        //qDebug()<<gradX[i]<<"  "<<fsgn(gradX[i]);
        fimgG[i] = hardT(sqrtf(gradX[i]*gradX[i]+gradY[i]*gradY[i]), gradTres);
        //fimgG[i] =sqrtf(gradX[i]*gradX[i]+gradY[i]*gradY[i]);
        
    }
    
    delete[] gradX;
    delete[] gradY;

}


#define grad(a,b,c) (((a>b)&&(b>c)) || ((a<b)&&(b<c)))

void LlorensSharpen(float *fimg, const unsigned int width, const unsigned int height,const int passes, const float strength){
	unsigned int i,j;
	unsigned int  offset;
	float *L, *fimgNew, *fimgOld;
	float lumH,lumV,lumD1,lumD2,s;
	float difL,difR,difT,difB,difLT,difRB,difLB,difRT,wH,wV,wD1,wD2;
	float f1,f2,f3,f4;
    //int size = width*height;
	const unsigned int width2=2*width;

	L = new float[width*height];
	memcpy(L,fimg,width*height*sizeof(float));  //init
	fimgOld = fimg;
	fimgNew = L;
	
	
	//sph mod see http://www.songho.ca/dsp/cannyedge/cannyedge.html
	// we'Re precalculating a gradient field and use its magnitude as a mask for the original llorens filter
	//this cuts down processing A LOT
	//the thresholding could use some tweaking
	    float *fimgG = new float[width*height];
	   /* 
	    float gradient[3] = {-0.5f,0.f,0.5f};
    float identity[1] = {1.f};
	    float *gradX = new float[width*height];
        float *gradY = new float[width*height];
       
        memcpy(fimgG,fimg,width*height*sizeof(float));
        
        USM_IIR(fimgG,width,height,1.0f+passes/4.f);
        convolve2DSeparable(fimgG,gradX,width,height,gradient,3,identity,1);
        convolve2DSeparable(fimgG,gradY,width,height,identity,1,gradient,3);
        const float gradTres = 0.0025f-(passes-2.5f)/10000.f;
        for(int i = 0; i<(width*height);i++) {
            //qDebug()<<gradX[i]<<"  "<<fsgn(gradX[i]);
            fimgG[i] = hardT(sqrtf(gradX[i]*gradX[i]+gradY[i]*gradY[i]), gradTres);
            //fimgG[i] =sqrtf(gradX[i]*gradX[i]+gradY[i]*gradY[i]);
            
        }
        
        delete[] gradX;
        delete[] gradY;*/
        edgeMask(fimg, fimgG, width,  height, passes);
	//---------------------
	

//	chmax=8.0f; //3.0 for color channels
	const float chmax=0.08f; // /100 because we use a 0..1 scale, original coe assumes 0..100
	for(int p=0;p<passes;p++) {
		for(j=2;j<height-2;j++)
		
		//maybe try moving the 4 filters into own loops, could boost locality
		    
			for(i=2,offset=j*width+i;i<width-2;i++,offset++) {
			
			  fimgNew[offset] = fimgOld[offset];
			  if(fimgG[offset] > 0.5f) 
			  {
			                //     yx
			    float x00 = fimgOld[offset-2-width2];
			    //float x01 =
			    float x02 = fimgOld[offset-width2];
			    //float x03 =
			    float x04 = fimgOld[offset-width2+2];
			    
			    //float x10 =
			    float x11 = fimgOld[offset-width-1];
			    float x12 = fimgOld[offset-width];
			    float x13 = fimgOld[offset-width+1];
			    //float x14 =
			    
			    float x20 = fimgOld[offset-2];
			    float x21 = fimgOld[offset-1];
			    float x22 = fimgOld[offset];
			    float x23 = fimgOld[offset+1];
			    float x24 = fimgOld[offset+2];
			    
			    //float x30 =
			    float x31 = fimgOld[offset+width-1];
			    float x32 = fimgOld[offset+width];
			    float x33 = fimgOld[offset+width+1];
			    //float x34 =
			    
			    float x40 = fimgOld[offset+width2-2];
			    //float x41 =
			    float x42 = fimgOld[offset+width2];
			    //float x43 =
			    float x44 = fimgOld[offset+2+width2];
			    
			    //fimgNew[offset] =  x22; //in doubt init with old value			    

			    
				// weight functions
				wH = FABS(x23-x21);
				wV = FABS(x32-x12);	

				s = /*1.0f*/0.001f+FABS(wH-wV)/2.0f;
				wD1 = FABS(x33-x11)/s;
				wD2 = FABS(x31-x13)/s;
				//s = wD1;
				if(wD2 !=0.f) wD1/=wD2; //fix by sph
				if(wD1 !=0.f) wD2/=wD1;
                
                if((wH!=0.f)&&(wV!=0.f)&&(wD1!=0.f)&&(wD2!=0.f)) { //don't mix if we don't have weights
				    // initial values
				    lumH=lumV=lumD1=lumD2=x22;					

				    // contrast	detection
				    float contrast=SQRT( wH*wH + wV*wV )/chmax;
				    if(contrast>1.0f) contrast=1.0f;


				    // new possible values //horizontal gradient
				    if( grad(x21,x22,x23) ){
					    f1=(x20-x21);
					    f2=(x21-x22);
					    f3=( (x21-x12) * (x21-x32) );
					    f4=SQRT( FABS( (x21-x02) * (x21-x42) ) );
					    difL=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x24-x23);
					    f2=(x23-x22);
					    f3=( (x23-x12) * (x23-x32) );
					    f4=SQRT( FABS( (x23-x02) * (x23-x42) ) );
					    difR=FABS(f1*f2*f2*f3*f3*f4);
					    if((difR!=0.f)&&(difL!=0.f)){
						    lumH=(x21*difR+x23*difL)/(difL+difR);
						    lumH=x22*(1.f-contrast)+lumH*contrast;
					    }
				    }
                    //vertical gradient
				    if( grad(x12,x22,x32) ){
					    f1=(x02-x12);
					    f2=(x12-x22);
					    f3=( (x12-x21) * (x12-x23) );
					    f4=SQRT( FABS( (x12-x20)*(x12-x24) ) );
					    difT=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x42-x32);
					    f2=(x32-x22);
					    f3=( (x32-x21) * (x32-x23) );
					    f4=SQRT( FABS( (x32-x20)*(x32-x24) ) );
					    difB=FABS(f1*f2*f2*f3*f3*f4);
					    if((difB!=0.f)&&(difT!=0.f)){
						    lumV=(x12*difB+x32*difT)/(difT+difB);
						    lumV=x22*(1.f-contrast)+lumV*contrast;
					    }
				    }
                    //diagonal gradient ltor down
				    if( grad(x11,x22,x33) ){
					    f1=(x00-x11);
					    f2=(x11-x22);
					    f3=((x11-x13) * (x11-x31));
					    f4=SQRT( FABS( (x11-x04) * (x11-x40) ) );
					    difLT=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x44-x33);
					    f2=(x33-x22);
					    f3=( (x33-x13)*(x33-x31));
					    f4=SQRT( FABS( (x33-x04) * (x33-x40) ) );
					    difRB=FABS(f1*f2*f2*f3*f3*f4);
					    if((difLT!=0.f)&&(difRB!=0.f)){
						    lumD1=(x11*difRB+x33*difLT)/(difLT+difRB);
						    lumD1=x22*(1.f-contrast)+lumD1*contrast;
					    }
				    }
                    //diagonal graient ltoR up
				    if( grad(x13,x22,x31) ){
					    f1=(x40-x31);
					    f2=(x31-x22);
					    f3=( (x31-x11) * (x31-x33) );
					    f4=SQRT(FABS( (x31-x00) * (x31-x44) ) );
					    difLB=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x04-x13);
					    f2=( (x13-x22) /** (x13-x22)*/ );
					    f3=( (x13-x33) * (x13-x11) );
					    f4=SQRT( FABS( (x13-x44) * (x13-x00) ) );
					    difRT=FABS(f1*f2*f2*f3*f3*f4);
					    if((difLB!=0.f)&&(difRT!=0.f)){
						    lumD2=(x13*difLB+x31*difRT)/(difLB+difRT);
						    lumD2=x22*(1.f-contrast)+lumD2*contrast;
					    }
				    }

				    s=strength/passes;					

				    // avoid sharpening diagonals too much
				   /* if((((wH/wV)<0.45f)&&((wH/wV)>0.05f))||(((wV/wH)<0.45f)&&((wV/wH)>0.05f)))
					    s=strength/3.0f;																	*/
                   /* if((((wH/wV)>0.45f)&&((wH/wV)<0.05f))||(((wV/wH)>0.45f)&&((wV/wH)<0.05f)))
					    s=strength/3.0f;*/
				
				
				    // final mix
                    fimgNew[offset]=x22*(1.f-s)+(lumH*wH+lumV*wV+lumD1*wD1+lumD2*wD2)/(wH+wV+wD1+wD2)*s;
			        
				    fimgNew[offset] = clipf(fimgNew[offset],0.0f,1.0f);
				    
				}
              } //end pixle will be modifieds
			}//end for all pixels loop
			
        //switch buffers			
        float* t = fimgNew;
        fimgNew = fimgOld;
        fimgOld = t; //newest image is now in old
	}
	
	if(fimgOld != fimg) memcpy(fimg,fimgOld,width*height*sizeof(float)); 
	#ifdef SHOWMASK
	memcpy(fimg,fimgG,width*height*sizeof(float));
	#endif
	delete[] L;
	delete[] fimgG;
} 
#endif



#ifndef SPH_VERSION

#define grad(a,b,c) (((a>b)&&(b>c)) || ((a<b)&&(b<c)))

void LlorensSharpen(float *fimg, const unsigned int width, const unsigned int height,const int passes, const float strength){
	unsigned int i,j;
	unsigned int  offset;
	float *L, *fimgNew, *fimgOld;
	float lumH,lumV,lumD1,lumD2,s;
	float difL,difR,difT,difB,difLT,difRB,difLB,difRT,wH,wV,wD1,wD2;
	float f1,f2,f3,f4;
    //int size = width*height;
	const unsigned int width2=2*width;

	L = new float[width*height];
	fimgOld = fimg;
	fimgNew = L;

//	chmax=8.0f; //3.0 for color channels
	const float chmax=0.08f; // /100 because we use a 0..1 scale, original coe assumes 0..100
	for(int p=0;p<passes;p++) {
		for(j=2;j<height-2;j++)
		
		//maybe try moving the 4 filters into own loops, could boost locality
			for(i=2,offset=j*width+i;i<width-2;i++,offset++){
			                //     yx
			    float x00 = fimgOld[offset-2-width2];
			    //float x01 =
			    float x02 = fimgOld[offset-width2];
			    //float x03 =
			    float x04 = fimgOld[offset-width2+2];
			    
			    //float x10 =
			    float x11 = fimgOld[offset-width-1];
			    float x12 = fimgOld[offset-width];
			    float x13 = fimgOld[offset-width+1];
			    //float x14 =
			    
			    float x20 = fimgOld[offset-2];
			    float x21 = fimgOld[offset-1];
			    float x22 = fimgOld[offset];
			    float x23 = fimgOld[offset+1];
			    float x24 = fimgOld[offset+2];
			    
			    //float x30 =
			    float x31 = fimgOld[offset+width-1];
			    float x32 = fimgOld[offset+width];
			    float x33 = fimgOld[offset+width+1];
			    //float x34 =
			    
			    float x40 = fimgOld[offset+width2-2];
			    //float x41 =
			    float x42 =fimgOld[offset+width2];
			    //float x43 =
			    float x44 = fimgOld[offset+2+width2];
			    
			    fimgNew[offset] =  x22; //in doubt init with old value			    

			    
				// weight functions
				wH = FABS(x23-x21);
				wV = FABS(x32-x12);	

				s = 1.0f+FABS(wH-wV)/2.0f;
				wD1 = FABS(x33-x11)/s;
				wD2 = FABS(x31-x13)/s;
				//s = wD1;
				if(wD2 !=0.f) wD1/=wD2; //fix by sph
				if(wD1 !=0.f) wD2/=wD1;
                
                if((wH!=0.f)&&(wV!=0.f)&&(wD1!=0.f)&&(wD2!=0.f)) { //don't mix if we don't have weights
				    // initial values
				    lumH=lumV=lumD1=lumD2=x22;					

				    // contrast	detection
				    float contrast=SQRT( wH*wH + wV*wV )/chmax;
				    if(contrast>1.0f) contrast=1.0f;

				    // new possible values //horizontal gradient
				    if( grad(x21,x22,x23) ){
					    f1=(x20-x21);
					    f2=(x21-x22);
					    f3=( (x21-x12) * (x21-x32) );
					    f4=SQRT( FABS( (x21-x02) * (x21-x42) ) );
					    difL=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x24-x23);
					    f2=(x23-x22);
					    f3=( (x23-x12) * (x23-x32) );
					    f4=SQRT( FABS( (x23-x02) * (x23-x42) ) );
					    difR=FABS(f1*f2*f2*f3*f3*f4);
					    if((difR!=0.f)&&(difL!=0.f)){
						    lumH=(x21*difR+x23*difL)/(difL+difR);
						    lumH=x22*(1.f-contrast)+lumH*contrast;
					    }
				    }
                    //vertical gradient
				    if( grad(x12,x22,x32) ){
					    f1=(x02-x12);
					    f2=(x12-x22);
					    f3=( (x12-x21) * (x12-x23) );
					    f4=SQRT( FABS( (x12-x20)*(x12-x24) ) );
					    difT=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x42-x32);
					    f2=(x32-x22);
					    f3=( (x32-x21) * (x32-x23) );
					    f4=SQRT( FABS( (x32-x20)*(x32-x24) ) );
					    difB=FABS(f1*f2*f2*f3*f3*f4);
					    if((difB!=0.f)&&(difT!=0.f)){
						    lumV=(x12*difB+x32*difT)/(difT+difB);
						    lumV=x22*(1.f-contrast)+lumV*contrast;
					    }
				    }
                    //diagonal gradient ltor down
				    if( grad(x11,x22,x33) ){
					    f1=(x00-x11);
					    f2=(x11-x22);
					    f3=((x11-x13) * (x11-x31));
					    f4=SQRT( FABS( (x11-x04) * (x11-x40) ) );
					    difLT=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x44-x33);
					    f2=(x33-x22);
					    f3=( (x33-x13)*(x33-x31));
					    f4=SQRT( FABS( (x33-x04) * (x33-x40) ) );
					    difRB=FABS(f1*f2*f2*f3*f3*f4);
					    if((difLT!=0.f)&&(difRB!=0.f)){
						    lumD1=(x11*difRB+x33*difLT)/(difLT+difRB);
						    lumD1=x22*(1.f-contrast)+lumD1*contrast;
					    }
				    }
                    //diagonal graient ltoR up
				    if( grad(x13,x22,x31) ){
					    f1=(x40-x31);
					    f2=(x31-x22);
					    f3=( (x31-x11) * (x31-x33) );
					    f4=SQRT(FABS( (x31-x00) * (x31-x44) ) );
					    difLB=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(x04-x13);
					    f2=( (x13-x22) /** (x13-x22)*/ );
					    f3=( (x13-x33) * (x13-x11) );
					    f4=SQRT( FABS( (x13-x44) * (x13-x00) ) );
					    difRT=FABS(f1*f2*f2*f3*f3*f4);
					    if((difLB!=0.f)&&(difRT!=0.f)){
						    lumD2=(x13*difLB+x31*difRT)/(difLB+difRT);
						    lumD2=x22*(1.f-contrast)+lumD2*contrast;
					    }
				    }

				    s=strength;					

				    // avoid sharpening diagonals too much
				    if((((wH/wV)<0.45f)&&((wH/wV)>0.05f))||(((wV/wH)<0.45f)&&((wV/wH)>0.05f)))
					    s=strength/3.0f;																	

				
				
				
				    // final mix
			        fimgNew[offset]=x22*(1.f-s)+(lumH*wH+lumV*wV+lumD1*wD1+lumD2*wD2)/(wH+wV+wD1+wD2)*s;
			        
				    fimgNew[offset] = clipf(fimgNew[offset],0.0f,1.0f);
				}

			}//end for all pixels loop
			
        //switch buffers			
        float* t = fimgNew;
        fimgNew = fimgOld;
        fimgOld = t;
	}
	
	if(fimgNew != fimg) memcpy(fimg,fimgNew,width*height*sizeof(float));	
	delete[] L;
}
#endif

#if 0

void LlorensSharpen(float *fimg, const unsigned int width, const unsigned int height,const int passes, const float strength){
	unsigned int i,j,width2;
	unsigned int  offset;
	float *L, *fimgNew, *fimgOld;
	float lumH,lumV,lumD1,lumD2,v,s;
	float difL,difR,difT,difB,difLT,difRB,difLB,difRT,wH,wV,wD1,wD2;
	float f1,f2,f3,f4;
    //int size = width*height;
	width2=2*width;

	L = new float[width*height];
	fimgOld = fimg;
	fimgNew = L;

//	chmax=8.0f; //3.0 for color channels
	const float chmax=0.08f; // /100 because we use a 0..1 scale, original coe assumes 0..100
	for(int p=0;p<passes;p++) {
		for(j=2;j<height-2;j++)
			for(i=2,offset=j*width+i;i<width-2;i++,offset++){
				// weight functions
				wH = FABS(fimgOld[offset+1]-fimgOld[offset-1]);
				wV = FABS(fimgOld[offset+width]-fimgOld[offset-width]);	

				s = 1.0f+FABS(wH-wV)/2.0f;
				wD1 = FABS(fimgOld[offset+width+1]-fimgOld[offset-width-1])/s;
				wD2 = FABS(fimgOld[offset+width-1]-fimgOld[offset-width+1])/s;
				s = wD1;
				if(wD2 !=0.f) wD1/=wD2; //fix by sph
				if(wD1 !=0.f) wD2/=wD1;
                
                if((wH!=0.f)&&(wV!=0.f)&&(wD1!=0.f)&&(wD2!=0.f)) { //don't mix if we don't have weights
				    // initial values
				    lumH=lumV=lumD1=lumD2=v=fimgOld[offset];					

				    // contrast	detection
				    float contrast=SQRT( wH*wH + wV*wV )/chmax;
				    if(contrast>1.0f) contrast=1.0f;

				    // new possible values //horizontal gradient
				    if( ( (fimgOld[offset]<fimgOld[offset-1])&&(fimgOld[offset]>fimgOld[offset+1]) ) || ( (fimgOld[offset]>fimgOld[offset-1])&&(fimgOld[offset]<fimgOld[offset+1]) ) ){
					    f1=(fimgOld[offset-2]-fimgOld[offset-1]);
					    f2=(fimgOld[offset-1]-fimgOld[offset]);
					    f3=( (fimgOld[offset-1]-fimgOld[offset-width]) * (fimgOld[offset-1]-fimgOld[offset+width]) );
					    f4=SQRT( FABS( (fimgOld[offset-1]-fimgOld[offset-width2]) * (fimgOld[offset-1]-fimgOld[offset+width2]) ) );
					    difL=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(fimgOld[offset+2]-fimgOld[offset+1]);
					    f2=(fimgOld[offset+1]-fimgOld[offset]);
					    f3=( (fimgOld[offset+1]-fimgOld[offset-width]) * (fimgOld[offset+1]-fimgOld[offset+width]) );
					    f4=SQRT( FABS( (fimgOld[offset+1]-fimgOld[offset-width2]) * (fimgOld[offset+1]-fimgOld[offset+width2]) ) );
					    difR=FABS(f1*f2*f2*f3*f3*f4);
					    if((difR!=0.f)&&(difL!=0.f)){
						    lumH=(fimgOld[offset-1]*difR+fimgOld[offset+1]*difL)/(difL+difR);
						    lumH=v*(1.f-contrast)+lumH*contrast;
					    }
				    }
                    //vertical gradient
				    if(((fimgOld[offset]<fimgOld[offset-width])&&(fimgOld[offset]>fimgOld[offset+width]))||((fimgOld[offset]>fimgOld[offset-width])&&(fimgOld[offset]<fimgOld[offset+width]))){
					    f1=(fimgOld[offset-width2]-fimgOld[offset-width]);
					    f2=(fimgOld[offset-width]-fimgOld[offset]);
					    f3=( (fimgOld[offset-width]-fimgOld[offset-1]) * (fimgOld[offset-width]-fimgOld[offset+1]) );
					    f4=SQRT( FABS( (fimgOld[offset-width]-fimgOld[offset-2])*(fimgOld[offset-width]-fimgOld[offset+2]) ) );
					    difT=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(fimgOld[offset+width2]-fimgOld[offset+width]);
					    f2=(fimgOld[offset+width]-fimgOld[offset]);
					    f3=( (fimgOld[offset+width]-fimgOld[offset-1]) * (fimgOld[offset+width]-fimgOld[offset+1]) );
					    f4=SQRT( FABS( (fimgOld[offset+width]-fimgOld[offset-2])*(fimgOld[offset+width]-fimgOld[offset+2]) ) );
					    difB=FABS(f1*f2*f2*f3*f3*f4);
					    if((difB!=0.f)&&(difT!=0.f)){
						    lumV=(fimgOld[offset-width]*difB+fimgOld[offset+width]*difT)/(difT+difB);
						    lumV=v*(1.f-contrast)+lumV*contrast;
					    }
				    }
                    //diagonal gradient ltor down
				    if(((fimgOld[offset]<fimgOld[offset-1-width])&&(fimgOld[offset]>fimgOld[offset+1+width]))||((fimgOld[offset]>fimgOld[offset-1-width])&&(fimgOld[offset]<fimgOld[offset+1+width]))){
					    f1=(fimgOld[offset-2-width2]-fimgOld[offset-1-width]);
					    f2=(fimgOld[offset-1-width]-fimgOld[offset]);
					    f3=((fimgOld[offset-1-width]-fimgOld[offset-width+1]) * (fimgOld[offset-1-width]-fimgOld[offset+width-1]));
					    f4=SQRT( FABS( (fimgOld[offset-1-width]-fimgOld[offset-width2+2]) * (fimgOld[offset-1-width]-fimgOld[offset+width2-2]) ) );
					    difLT=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(fimgOld[offset+2+width2]-fimgOld[offset+1+width]);
					    f2=(fimgOld[offset+1+width]-fimgOld[offset]);
					    f3=( (fimgOld[offset+1+width]-fimgOld[offset-width+1])*(fimgOld[offset+1+width]-fimgOld[offset+width-1]));
					    f4=SQRT( FABS( (fimgOld[offset+1+width]-fimgOld[offset-width2+2]) * (fimgOld[offset+1+width]-fimgOld[offset+width2-2]) ) );
					    difRB=FABS(f1*f2*f2*f3*f3*f4);
					    if((difLT!=0.f)&&(difRB!=0.f)){
						    lumD1=(fimgOld[offset-1-width]*difRB+fimgOld[offset+1+width]*difLT)/(difLT+difRB);
						    lumD1=v*(1.f-contrast)+lumD1*contrast;
					    }
				    }
                    //diagonal graient ltoR up
				    if(((fimgOld[offset]<fimgOld[offset+1-width])&&(fimgOld[offset]>fimgOld[offset-1+width]))||((fimgOld[offset]>fimgOld[offset+1-width])&&(fimgOld[offset]<fimgOld[offset-1+width]))){
					    f1=(fimgOld[offset-2+width2]-fimgOld[offset-1+width]);
					    f2=(fimgOld[offset-1+width]-fimgOld[offset]);
					    f3=( (fimgOld[offset-1+width]-fimgOld[offset-width-1]) * (fimgOld[offset-1+width]-fimgOld[offset+width+1]) );
					    f4=SQRT(FABS( (fimgOld[offset-1+width]-fimgOld[offset-width2-2]) * (fimgOld[offset-1+width]-fimgOld[offset+width2+2]) ) );
					    difLB=FABS(f1*f2*f2*f3*f3*f4);
					    f1=(fimgOld[offset+2-width2]-fimgOld[offset+1-width]);
					    f2=( (fimgOld[offset+1-width]-fimgOld[offset]) * (fimgOld[offset+1-width]-fimgOld[offset]) );
					    f3=( (fimgOld[offset+1-width]-fimgOld[offset+width+1]) * (fimgOld[offset+1-width]-fimgOld[offset-width-1]) );
					    f4=SQRT( FABS( (fimgOld[offset+1-width]-fimgOld[offset+width2+2]) * (fimgOld[offset+1-width]-fimgOld[offset-width2-2]) ) );
					    difRT=FABS(f1*f2*f2*f3*f3*f4);
					    if((difLB!=0.f)&&(difRT!=0.f)){
						    lumD2=(fimgOld[offset+1-width]*difLB+fimgOld[offset-1+width]*difRT)/(difLB+difRT);
						    lumD2=v*(1.f-contrast)+lumD2*contrast;
					    }
				    }

				    s=strength;					

				    // avoid sharpening diagonals too much
				    if((((wH/wV)<0.45f)&&((wH/wV)>0.05f))||(((wV/wH)<0.45f)&&((wV/wH)>0.05f)))
					    s=strength/3.0f;																	

				
				
				
				    // final mix
			        fimgNew[offset]=v*(1.f-s)+(lumH*wH+lumV*wV+lumD1*wD1+lumD2*wD2)/(wH+wV+wD1+wD2)*s;
			        
				    fimgNew[offset] = clipf(fimgNew[offset],0.0f,1.0f);
				}
				else  fimgNew[offset] =  fimgOld[offset];

			}//end for all pixels loop
			
        //switch buffers			
        float* t = fimgNew;
        fimgNew = fimgOld;
        fimgOld = t;
	}
	
	if(fimgNew != fimg) memcpy(fimg,fimgNew,width*height*sizeof(float));	
	delete[] L;
}
#endif
#if 0
void LlorensSharpen(float *fimg, const unsigned int width, const unsigned int height,const int passes, const float strength){
	unsigned int i,j,width2;
	unsigned int  offset;
	float *L;
	float lumH,lumV,lumD1,lumD2,v,s;
	float difL,difR,difT,difB,difLT,difRB,difLB,difRT,wH,wV,wD1,wD2,chmax;
	float f1,f2,f3,f4;
    //int size = width*height;
	width2=2*width;

	L = new float[width*height];

    //TODO: lots of duplicated fabs ( fabs(A)*fabs(B) == fabs(A*B)
//	chmax=8.0f; //3.0 for color channels
	chmax=0.08f; ///100 because we use a 0..1 scale, original coe assumes 0..100
	for(int p=0;p<passes;p++){

		/*for(offset=0;offset<width*height;offset++)
			L[offset]=fimg[offset];*/
		memcpy(L,fimg,width*height*sizeof(float));	


		for(j=2;j<height-2;j++)
			for(i=2,offset=j*width+i;i<width-2;i++,offset++){
				// weight functions
				wH = fabs(L[offset+1]-L[offset-1]);
				wV = fabs(L[offset+width]-L[offset-width]);	

				s = 1.0f+fabs(wH-wV)/2.0f;
				wD1 = fabs(L[offset+width+1]-L[offset-width-1])/s;
				wD2 = fabs(L[offset+width-1]-L[offset-width+1])/s;
				s = wD1;
				if(wD2 !=0.f) wD1/=wD2; //fix by sph
				if(wD1 !=0.f) wD2/=wD1;
                
                if((wH!=0.f)&&(wV!=0.f)&&(wD1!=0.f)&&(wD2!=0.f)) { //don't mix if we don't have weights
				    // initial values
				    lumH=lumV=lumD1=lumD2=v=fimg[offset];					

				    // contrast	detection
				    float contrast=sqrt( wH*wH + wV*wV )/chmax;
				    if(contrast>1.0f) contrast=1.0f;

				    // new possible values //horizontal gradient
				    if( ( (L[offset]<L[offset-1])&&(L[offset]>L[offset+1]) ) || ( (L[offset]>L[offset-1])&&(L[offset]<L[offset+1]) ) ){
					    f1=(L[offset-2]-L[offset-1]);
					    f2=(L[offset-1]-L[offset]);
					    f3=( (L[offset-1]-L[offset-width]) * (L[offset-1]-L[offset+width]) );
					    f4=sqrt( fabs( (L[offset-1]-L[offset-width2]) * (L[offset-1]-L[offset+width2]) ) );
					    difL=fabs(f1*f2*f2*f3*f3*f4);
					    f1=(L[offset+2]-L[offset+1]);
					    f2=(L[offset+1]-L[offset]);
					    f3=( (L[offset+1]-L[offset-width]) * (L[offset+1]-L[offset+width]) );
					    f4=sqrt( fabs( (L[offset+1]-L[offset-width2]) * (L[offset+1]-L[offset+width2]) ) );
					    difR=fabs(f1*f2*f2*f3*f3*f4);
					    if((difR!=0.f)&&(difL!=0.f)){
						    lumH=(L[offset-1]*difR+L[offset+1]*difL)/(difL+difR);
						    lumH=v*(1.f-contrast)+lumH*contrast;
					    }
				    }
                    //vertical gradient
				    if(((L[offset]<L[offset-width])&&(L[offset]>L[offset+width]))||((L[offset]>L[offset-width])&&(L[offset]<L[offset+width]))){
					    f1=(L[offset-width2]-L[offset-width]);
					    f2=(L[offset-width]-L[offset]);
					    f3=( (L[offset-width]-L[offset-1]) * (L[offset-width]-L[offset+1]) );
					    f4=sqrt( fabs( (L[offset-width]-L[offset-2])*(L[offset-width]-L[offset+2]) ) );
					    difT=fabs(f1*f2*f2*f3*f3*f4);
					    f1=(L[offset+width2]-L[offset+width]);
					    f2=(L[offset+width]-L[offset]);
					    f3=( (L[offset+width]-L[offset-1]) * (L[offset+width]-L[offset+1]) );
					    f4=sqrt( fabs( (L[offset+width]-L[offset-2])*(L[offset+width]-L[offset+2]) ) );
					    difB=fabs(f1*f2*f2*f3*f3*f4);
					    if((difB!=0.f)&&(difT!=0.f)){
						    lumV=(L[offset-width]*difB+L[offset+width]*difT)/(difT+difB);
						    lumV=v*(1.f-contrast)+lumV*contrast;
					    }
				    }
                    //diagonal gradient ltor down
				    if(((L[offset]<L[offset-1-width])&&(L[offset]>L[offset+1+width]))||((L[offset]>L[offset-1-width])&&(L[offset]<L[offset+1+width]))){
					    f1=(L[offset-2-width2]-L[offset-1-width]);
					    f2=(L[offset-1-width]-L[offset]);
					    f3=((L[offset-1-width]-L[offset-width+1]) * (L[offset-1-width]-L[offset+width-1]));
					    f4=sqrt( fabs( (L[offset-1-width]-L[offset-width2+2]) * (L[offset-1-width]-L[offset+width2-2]) ) );
					    difLT=fabs(f1*f2*f2*f3*f3*f4);
					    f1=(L[offset+2+width2]-L[offset+1+width]);
					    f2=(L[offset+1+width]-L[offset]);
					    f3=( (L[offset+1+width]-L[offset-width+1])*(L[offset+1+width]-L[offset+width-1]));
					    f4=sqrt( fabs( (L[offset+1+width]-L[offset-width2+2]) * (L[offset+1+width]-L[offset+width2-2]) ) );
					    difRB=fabs(f1*f2*f2*f3*f3*f4);
					    if((difLT!=0.f)&&(difRB!=0.f)){
						    lumD1=(L[offset-1-width]*difRB+L[offset+1+width]*difLT)/(difLT+difRB);
						    lumD1=v*(1.f-contrast)+lumD1*contrast;
					    }
				    }
                    //diagonal graient ltoR up
				    if(((L[offset]<L[offset+1-width])&&(L[offset]>L[offset-1+width]))||((L[offset]>L[offset+1-width])&&(L[offset]<L[offset-1+width]))){
					    f1=(L[offset-2+width2]-L[offset-1+width]);
					    f2=(L[offset-1+width]-L[offset]);
					    f3=( (L[offset-1+width]-L[offset-width-1]) * (L[offset-1+width]-L[offset+width+1]) );
					    f4=sqrt(fabs( (L[offset-1+width]-L[offset-width2-2]) * (L[offset-1+width]-L[offset+width2+2]) ) );
					    difLB=fabs(f1*f2*f2*f3*f3*f4);
					    f1=(L[offset+2-width2]-L[offset+1-width]);
					    f2=( (L[offset+1-width]-L[offset]) * (L[offset+1-width]-L[offset]) );
					    f3=( (L[offset+1-width]-L[offset+width+1]) * (L[offset+1-width]-L[offset-width-1]) );
					    f4=sqrt( fabs( (L[offset+1-width]-L[offset+width2+2]) * (L[offset+1-width]-L[offset-width2-2]) ) );
					    difRT=fabs(f1*f2*f2*f3*f3*f4);
					    if((difLB!=0.f)&&(difRT!=0.f)){
						    lumD2=(L[offset+1-width]*difLB+L[offset-1+width]*difRT)/(difLB+difRT);
						    lumD2=v*(1.f-contrast)+lumD2*contrast;
					    }
				    }

				    s=strength;					

				    // avoid sharpening diagonals too much
				    if((((wH/wV)<0.45f)&&((wH/wV)>0.05f))||(((wV/wH)<0.45f)&&((wV/wH)>0.05f)))
					    s=strength/3.0f;																	

				
				
				
				    // final mix
			        fimg[offset]=v*(1.f-s)+(lumH*wH+lumV*wV+lumD1*wD1+lumD2*wD2)/(wH+wV+wD1+wD2)*s;
			        
				    fimg[offset] = clipf(fimg[offset],0.0f,1.0f);
				}
					
				//if(fimg[offset] >=1.0f) qDebug()<<lumH<<" "<<wH<<" "<<lumV<<" "<<wV<<" "<<lumD1<<" "<<wD1<<" "<<lumD2<<" "<<wD2<<" s:"<<s<<" "<<L[offset+width+1]<<" "<<L[offset-width-1]<<" "<<L[offset+width-1]<<" "<<L[offset-width+1];
			}//end for all pixels loop
	}

	delete[] L;
}
#endif


// To the extent possible under law, Manuel Llorens <manuelllorens@gmail.com>
// has waived all copyright and related or neighboring rights to this work.
// This code is licensed under CC0 v1.0, see license information at
// http://creativecommons.org/publicdomain/zero/1.0/

void microcontrast(float *fimg, const unsigned int width, const unsigned int height,float strength){
	int offset2,c,col,row,n;
	unsigned int i,j;
	unsigned int  offset;
	float *L,v,s,contrast;
	int signs[9];

        L = new float[width*height];
	

	c=0;

	for(offset=0;offset<width*height;offset++)
		L[offset]=fimg[offset];


	for(j=1;j<height-1;j++)
		for(i=1,offset=j*width+i;i<width-1;i++,offset++){
			s=strength;
			v=L[offset];			
//todo, remove all the signs crap
			n=0;
			for(row=j-1;row<=j+1;row++)
				for(col=i-1,offset2=row*width+col;col<=i+1;col++,offset2++){
					signs[n]=0;
					if(v<L[offset2]) signs[n]=-1;
					if(v>L[offset2]) signs[n]=1;
					n++;
				}	

			contrast=SQRT(FABS(L[offset+1]-L[offset-1])*FABS(L[offset+1]-L[offset-1])+FABS(L[offset+width]-L[offset-width])*FABS(L[offset+width]-L[offset-width]))/ 0.08f;//8.0;
			if(contrast>1.0f) contrast=1.0f;			

			fimg[offset]+=(v-L[offset-width-1])*SQRT(2.f)*s;
			fimg[offset]+=(v-L[offset-width])*s;
			fimg[offset]+=(v-L[offset-width+1])*SQRT(2.f)*s;

			fimg[offset]+=(v-L[offset-1])*s;
			fimg[offset]+=(v-L[offset+1])*s;

			fimg[offset]+=(v-L[offset+width-1])*SQRT(2.f)*s;
			fimg[offset]+=(v-L[offset+width])*s;
			fimg[offset]+=(v-L[offset+width+1])*SQRT(2.f)*s;

			if(fimg[offset]<0) fimg[offset]=0;			

			// Reduce halo looking artifacs 
			
			v=fimg[offset];
			n=0;
			for(row=j-1;row<=j+1;row++)
				for(col=i-1,offset2=row*width+col;col<=i+1;col++,offset2++){
					if(((v<L[offset2])&&(signs[n]>0))||((v>L[offset2])&&(signs[n]<0)))
						fimg[offset]=v*0.75f+L[offset2]*0.25f;
					n++;
				}
			
			fimg[offset]=fimg[offset]*(1-contrast)+L[offset]*contrast;
			fimg[offset] = clipf(fimg[offset],0.0f,1.0f);
		}
	delete[] L;
}

//=====================================================
