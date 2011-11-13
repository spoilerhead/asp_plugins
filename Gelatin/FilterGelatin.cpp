#include "FilterGelatin.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"

#include "colorspace.h"
#include "blend_modes.h"

#include "CFilter.h"
#include "Interpolation.h"
#include "DataFilm.h"
#include "DataFilter.h"
#include "DataDeveloper.h"
#include "DataPaper.h"


#include <QDebug>
#include <iostream>

#define FILTERNAME GelatinFilter

/*inline float sign(const float value) {
    return (value<0.f)?1.f:1.f;//copysignf(1.0f,value);

}*/

//looks good, but the error is a bit high
inline float arctan2(const float y, const float x)
{
   float  angle;
   const float coeff_1 = M_PI/4.f;
   const float coeff_2 = 3.f*coeff_1;
   float abs_y = fabsf(y)+1e-10;      // kludge to prevent 0/0 condition
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

//redefine here for faster versions
//#define ATAN2 atan2#
#define ATAN2 arctan2


//TODO: LOOK AT : http://lists.apple.com/archives/perfoptimization-dev/2005/Jan/msg00051.html

//local used functions that should be inlined
//undef to get meaningfull debug symbols
#define FASTLOCAL static inline

#ifndef FASTLOCAL
#define FASTLOCAL
#endif

//TODO: remove as many conditional branches as possible

using namespace std;

//names to id
int getFilmID(const QString &name) {
    for(int i = 0; i<NUM_FILMS;i++) if(name == films[i].name) return i;
    
    return 0;
}


int getColorFilterID(const QString &name) {
    for(int i = 0; i<NUM_FILTERS;i++) if(name == cfilters[i].name) return i;
    
    return 0;
}

int getDeveloperID(const QString &name){
    for(int i = 0; i<NUM_DEV;i++) if(name == developers[i].name) return i+1; //+1, doesn't contain a NONE element
    
    return 0;
}

int getPaperID(const QString &name){
    for(int i = 0; i<NUM_PAPERS;i++) if(name == papers[i].name) return i+1; //+1, doesn't contain a NONE element
    
    return 0;
}


//combo box filler

void fillColorFiltersCombo(QComboBox *p_combo) {
    if(p_combo == NULL) return; //NullPointer check
	for(int i = 0; i<NUM_FILTERS;i++) p_combo ->addItem(cfilters[i].name);//.c_str());
	p_combo->setCurrentIndex(0);
}

void fillFilmStockCombo(QComboBox *p_combo) {
    if(p_combo == NULL) return; //NullPointer check
	for(int i = 0; i<NUM_FILMS;i++) p_combo ->addItem(films[i].name);//.c_str());
	p_combo->setCurrentIndex(0);
}


void fillDeveloperCombo(QComboBox *p_combo) {
    if(p_combo == NULL) return; //NullPointer check
	p_combo ->addItem("None");
	for(int i = 0; i<NUM_DEV;i++) p_combo ->addItem(developers[i].name);//.c_str());
	p_combo->setCurrentIndex(0);
}

void fillPaperCombo(QComboBox *p_combo) {
    if(p_combo == NULL) return; //NullPointer check
	p_combo ->addItem("None");
	for(int i = 0; i<NUM_PAPERS;i++) p_combo ->addItem(papers[i].name);//.c_str());
	p_combo->setCurrentIndex(0);
}

//================================================================================

bool FILTERNAME::needsToRunLayer(const ImageSettings  &options, const PipeSettings  &settings, PluginOptionList &layerOptions) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	bool okay;
	bool val = layerOptions.getBool(FILTERNAME::Enable, m_groupId, okay);
	return val;
}

bool FILTERNAME::isSlow(const ImageSettings  &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;
}

bool FILTERNAME::isLargeRadius(const ImageSettings  &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;	//	We only use the current tile.
}

bool FILTERNAME::isPixelSource(const ImageSettings  &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;	//	We don't generate pixels from scratch.
}

QTransform FILTERNAME::transform(const ImageSettings  &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return QTransform();	//	We don't warp the image so return an identity transform.
}

QSize FILTERNAME::size(const ImageSettings  &options, const PipeSettings  &settings, const QSize &startSize) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return startSize;	//	We don't change the size of the image.
}

QList<PluginDependency*> FILTERNAME::prerequisites(const ImageSettings  &options, const PipeSettings  &settings, PluginTile &tile) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	Q_UNUSED( tile );
	
	return QList<PluginDependency*>();	//	Empty list - we don't depend on any other tiles.
}






FASTLOCAL pixelLABf applyFilter(pixelLABf px, const CFilter &filter, /*const float fSboost,*/ const bool baseTint/*, const float tintBoost*/ /*, const bool colorShift */) {
    float fH,fC,fS;
//compute missing L*C*H* values
	fH = ATAN2(px.B,px.A); //Hue (-pi..+pi)
	fC = sqrt(px.A*px.A+px.B*px.B); //chroma (saturation) (0..1)
	
	fS = fC; //fixed to mode 2
	//fS *= fSboost ;

    //lut index
	float fHindex =  ((fH/(2.f*M_PI))+0.5f)*SPECTRAL_POINTS; //20 degree steps;
	int iHindex   = fHindex;
	float fractpart = fHindex - iHindex;

    //get luminance scaler
	float scaler = HermiteInterpolate(
                    filter.filter.spectral[(iHindex-1+SPECTRAL_POINTS)%SPECTRAL_POINTS],
                    filter.filter.spectral[iHindex],
                    filter.filter.spectral[(iHindex+1)%SPECTRAL_POINTS],
                    filter.filter.spectral[(iHindex+2)%SPECTRAL_POINTS],
                    fractpart,
                    0.8f /*tension*/,
                    0.f /*bias*/); 
	scaler = scaler*filter.invAvg; //normalize
    //blend new luminance and old one
    //the assumption is, that each color is composed of scaler % monospectral color and (1-scaler)% white- white gets weighted by the spectras average (=transmission)

	px.L = BLEND(px.L*scaler,px.L, fS);
	//px.L = clipf(px.L,0.f,1.f);
	px.A = BLEND(px.A*scaler,px.A, fS);
	px.B = BLEND(px.B*scaler,px.B, fS); //color saturation gets modified, too
	
	if(baseTint) {
        /* tint affects the white part of the pixel
            bigger fS means theres a bigger monospectral part
        */
        //this method doesn't work that bad, but its actually still broken for extreme tints, compare with
        //filter gel
        //for extreme tints skipp the add and use the plain tint
        px.A = (filter.extremeTint?0.0f:px.A)/*Original tint*/*(fC)/*spectral part*/+ (px.A +(filter.TintA /**tintBoost*/))*(1.f-fC); /*white part*/
        px.B = (filter.extremeTint?0.0f:px.B) /*Original tint*/*(fC)/*spectral part*/+ (px.B +(filter.TintB /**tintBoost*/))*(1.f-fC); /*white part*/
        
        px.A = clipf(px.A,-1.f,1.f);
        px.B = clipf(px.B,-1.f,1.f); 
    }
    
  /*  if(colorShift) {
        //Color shift (for color films)
        float shiftA = LinearInterpolate(filter.filter.colorShiftA[0],
                                         filter.filter.colorShiftA[1],px.L);
        float shiftB = LinearInterpolate(filter.filter.colorShiftB[0],
                                         filter.filter.colorShiftB[1],px.L);
        px.A += shiftA;
        px.B += shiftB;
        
        px.A = clipf(px.A,-1.f,1.f);
        px.B = clipf(px.B,-1.f,1.f); 
    }*/
    return px;
}

#define BETTER_INTERPOLATION


FASTLOCAL float doContrastFilm(const contrastType data, float L) {
    
    /*float dmax = data.points[9];
    float dmin = data.points[0];
   
    int idx = L*8.f;
	float mu = L*8.f-idx;
	
	if(idx <0) L = dmin;       //to dark,
	else if(idx >8) L= dmax;  //to bright*/
	
	//alternative method, presers _some_ very minor details in highlights and shadows
	float dmax = data.points[9];
    float dmin = data.points[0];
    float dLO = data.points[1]-data.points[0]; //difference per shadow step, positive value
    float dHI = data.points[9]-data.points[8]; //difference per light step, positive value

    float Lscaled = L*8.f; //scale 0..1 to 0..8 (range of data points)
    int idx = Lscaled;
	float mu = Lscaled-idx;
	
	if(idx <0)      L = dmin+Lscaled*dLO;       //to dark, L <0
	else if(idx >8) L = dmax+  (Lscaled-9.f)*dHI;  //to bright, L>1 (on film, == bright on result)

		else
		#ifdef BETTER_INTERPOLATION 
		    L = HermiteInterpolate(
	                            ((idx>0)?data.points[idx-1]:dmin-dLO),
	                            data.points[idx],
	                            data.points[idx+1],
	                            ((idx<8)?data.points[idx+2]:dmax+dHI),mu,
	                            0.0f,0.f);
	    #else
	        L = LinearInterpolate(data.points[idx],data.points[idx+1],mu);
	    #endif
    L += data.base;
    L += (1.f-(dmax-dmin))*.5f; //normalize towards gray

    return L;
}


FASTLOCAL float doContrastPaper(const contrastType data,float L/*,const bool divideMax = false*/) {
    float dmax = data.points[9];
    float dmin = data.points[0];
    /*
    int idx = L*8.f;
	float mu = L*8.f-idx;
	
	if(idx <0) L = dmin;       //to dark,
	else if(idx >8) L= dmax;  //to bright*/
	
	//alternative method, preserves _some_ very minor details in highlights and shadows
    float dLO = data.points[1]-data.points[0]; //difference per shadow step, positive value
    float dHI = data.points[9]-data.points[8]; //difference per light step, positive value

    float Lscaled = L*8.f;//scale 0..1 to 0..8 (range of data points)
    int idx = Lscaled;
	float mu = Lscaled-idx;
	
	if(idx <0) L = dmin+Lscaled*dLO;       //to dark, L <0
	else if(idx >8) L= dmax+  (Lscaled-9.f)*dHI;  //to bright, L>1 (on paper, == dark on result)

	else
		#ifdef BETTER_INTERPOLATION 
		    L = HermiteInterpolate(
	                            ((idx>0)?data.points[idx-1]:dmin-dLO),
	                            data.points[idx],
	                            data.points[idx+1],
	                            ((idx<8)?data.points[idx+2]:dmax+dHI),mu,
	                            0.0f,0.f);
	    #else
	        L = LinearInterpolate(data.points[idx],data.points[idx+1],mu);
	    #endif
    L -= data.base;
    //float mid = data.points[4];
    //if(divideMax) {
        L /= (data.points[9]-data.base); 
    //}
    
    return L;
}

FASTLOCAL const float evToLabFactor(const float ev)  {
    return pow(
                pow(2.f,ev),        //EV to linear scale
                1.f/3.f)-1.f;           //linear factor to lab factor
}

typedef struct  {
    int developerID;        //user given parameter
    int paperID;            //user given parameter
    float expCorrFilm;      //user given parameter
    float expCorrPaper;     //user given parameter
    float expProfDev;       //profiled correction factor
    float expProfPaper;     //profiled correction factor
} optsDevPipe;


//#define TESTPATTERN

#define CONTRAST_SCALER 0.85f
FASTLOCAL pixelLABf developerPipe(pixelLABf px, const optsDevPipe opts) {
	//TODO: scaling factor for a+b (film saturation)
	//IDEA: real color film would need dye emulation, i.e. 3-4 filters
            
    // ======== Film Developer (contrast) =============================			
    px.L = px.L+opts.expCorrFilm; //exposure correction
    px.L = ((px.L-0.5f)*CONTRAST_SCALER)+0.5f+(1.f-CONTRAST_SCALER)*.5f; //x9 reduce contrast for this stage
	if(opts.developerID > 0) {
	    px.L = px.L+opts.expProfDev; //compress highlights
	    px.L = doContrastFilm(developers[opts.developerID-1],px.L);
	}
    //we can be very flat here //TODO
    px.L = 1.f-px.L; //invert; light are now at 0
//    px.A *= -1.f;
//    px.B *= -1.f;
    //we got a negative now
    
	// ======== Paper =================================================
	px.L = px.L+opts.expCorrPaper; //exposure correction
    px.L = ((px.L-0.5f)*CONTRAST_SCALER)+0.5f+(1.f-CONTRAST_SCALER)*.5f; //x9 reduce contrast for this stage
	if(opts.paperID > 0) {
	    px.L = px.L+opts.expProfPaper; //exposure correction
	    px.L = doContrastPaper(papers[opts.paperID-1],(px.L)/*,true*/);
	}
    px.L = 1.f-px.L; //invert; lighst are now back at 1
//	px.A *= -1.f;
//	px.B *= -1.f;  
			
	return px;
}


struct myPluginOptions {
    bool color;
    int filmStock;
    //float fSboost;
    //float tintBoost;
    bool baseTint;
    int colorFilter;
//    bool colorShift;
    int developer;
    int paper;
    float expPaper;
    float expFilm;
    bool stretchcontrast;
    float colorbleed;
};


void FILTERNAME::runLayer(const ImageSettings  &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList &layerOptions, int layerPos) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	//Q_UNUSED( layerOptions );
	
	PluginImage *	pSrcImg = tile.image();
	PluginImage *	pDstImg;
	
	//	Layer 0 output should always go into tile.image(), otherwise output should go into tile.tmpImage()
	if ( layerPos == 0 )
		pDstImg = tile.image();
	else
		pDstImg = tile.tmpImage();
		
	if((pSrcImg == NULL) || (pDstImg == NULL)) return; //NULLPointer check for safety
	
	int rw = pSrcImg->rowWords();	//	number of unsigned shorts in a row 
	int pw = pSrcImg->planeWords();	//	increment to get to the next color component in the plane
	int np = pSrcImg->nextPixel();	//	increment for the next pixel within the same color plane - if the pointer is
									//	pointing at a green pixel this incremet will point to the next green pixel

	//	Use paddedWidth & paddedHeight to get the padded width & height of the tile.
	//	We could also use image->paddedWidth() & image->paddedHeight().
	int tileWidth = settings.paddedWidth();        //Tile Dimensions
	int tileHeight = settings.paddedHeight();

	uint16 * pSrc = NULL;
	uint16 * pOut = NULL;
	
	
	uint16 iL,iA,iB;
	pixelLABf px,pxOrg;
	bool okay;
	struct myPluginOptions opts;
	
	// ==== Read Options ======================================================

    opts.stretchcontrast = layerOptions.getBool(StretchContrast, m_groupId, okay);
    
	//opts.fs = layerOptions.getInt(fSat, m_groupId, okay); //
	//opts.interpolate = layerOptions.getInt(Int, m_groupId, okay); // 	

/*    opts.colorFilter = layerOptions.getInt(ColorFilter, m_groupId, okay);
	opts.filmStock = layerOptions.getInt(FilmStock, m_groupId, okay); 
	opts.developer = layerOptions.getInt(Developer, m_groupId, okay);
	opts.paper = layerOptions.getInt(Paper, m_groupId, okay);*/		
    
    opts.colorFilter = getColorFilterID(layerOptions.getString(ColorFilter, m_groupId, okay));
	opts.filmStock   = getFilmID       (layerOptions.getString(FilmStock, m_groupId, okay)); 
	opts.developer   = getDeveloperID  (layerOptions.getString(Developer, m_groupId, okay));
	opts.paper       = getPaperID      (layerOptions.getString(Paper, m_groupId, okay));

	opts.expPaper   = -/*invert so it behaves like film*/layerOptions.getDouble(EXPPaper, m_groupId, okay);
    opts.expFilm    = layerOptions.getDouble(EXPFilm, m_groupId, okay);

	opts.color      = layerOptions.getBool(Color, m_groupId, okay); //color mode?
    opts.baseTint   = layerOptions.getBool(BaseTint, m_groupId, okay);	
    		
	//opts.fSboost = layerOptions.getInt(IntensityBoost, m_groupId, okay)/100.f;
    //opts.tintBoost = layerOptions.getInt(TintBoost, m_groupId, okay)/100.f;
	//opts.colorShift = layerOptions.getBool(ColorShift, m_groupId, okay);
    opts.colorbleed = layerOptions.getInt(Colorbleed, m_groupId, okay)/100.f;
	


	
	// ========================================================================
	
	// == Spectral Filters
	CFilter filmFilter(films[opts.filmStock]);
	CFilter colorfilter(cfilters[opts.colorFilter]);
	
	// == Developer Pipe options (mostly precomputed values)
	optsDevPipe devOpts;
	devOpts.developerID  = opts.developer;
	devOpts.paperID      = opts.paper;
	devOpts.expCorrFilm  = evToLabFactor(opts.expFilm);
	devOpts.expCorrPaper = evToLabFactor(opts.expPaper);
	devOpts.expProfDev   = evToLabFactor(developers[opts.developer-1].expMult);
	devOpts.expProfPaper = evToLabFactor(papers[opts.paper-1].expMult);

	// ========
	
	// == get extremes, for contrast equalization
	pixelLABf pxWhite = {1.f,0.f,0.f};
    pixelLABf pxBlack = {0.f,0.f,0.f};
	pxWhite = developerPipe(pxWhite,devOpts);
	pxBlack = developerPipe(pxBlack,devOpts);
	const float contrastFactorL = 1.f/(pxWhite.L-pxBlack.L); //factor needed to scale the range to 0..1, precompute this
	
	for ( int y = 0; y < tileHeight; y++ ) {
		//	image-data16() points to the very first unpadded pixel
		//	add to that (rw * y) to point us at the first unpadded pixel for line 'y'
		pSrc = pSrcImg->data16() + rw * y;
		pOut = pDstImg->data16() + rw * y;
		
		for ( int x = 0; x < tileWidth ; x++, pOut += np, pSrc += np ) { //	move to the next pixel (in the color plane)
		    // == read from image plane
			iL = *(pSrc)        ;
			iA = *(pSrc + pw)   ;
			iB = *(pSrc + 2*pw) ;
#ifdef TESTPATTERN
			iL=x*y; //FIXME: test pattern
#endif			
           // for(int bla = 0; bla<20;bla++) {
			// == to float space
			px.L = I16TOF(iL);
			px.A = (I16TOF(iA)-0.5f)*2.f;
			px.B = (I16TOF(iB)-0.5f)*2.f;
			
			//store original pixel
			pxOrg.L = px.L;
			pxOrg.A = px.A;
			pxOrg.B = px.B;

            // ======== Color Filter ==========================================
			if(opts.colorFilter != 0) px = applyFilter(px, colorfilter,/* opts.fSboost,*/ true/*,          opts.tintBoost*//*, true*/); //always apply tint and shift to filters (as the affect the film result)

            // ======== Film Stock ============================================
			if(opts.filmStock != 0)   px = applyFilter(px, filmFilter,  /*opts.fSboost,*/ opts.baseTint/*, opts.tintBoost*//*, opts.colorShift*/);

			
            px = developerPipe(px,devOpts);
			
			if(opts.stretchcontrast) px.L = (px.L-pxBlack.L)*contrastFactorL; //normalize
			
			
			
			
			//Clip outputs before converting back to int space
			px.L = clipf(px.L,0.f,1.f);
			px.A = clipf(px.A,-1.f,1.f);
			px.B = clipf(px.B,-1.f,1.f);
			//} //end bla
			//TODO: color bleedthrough opts.colorbleed
            
            // == grayscale conversion
			if(!opts.color) {
			    px.A = 0.f; px.B=0.f;
			}
            
            px.A = BLEND(pxOrg.A,px.A,opts.colorbleed);
            px.B = BLEND(pxOrg.B,px.B,opts.colorbleed);
            
			//back to integer space
			iL = FTOI16(px.L);
			iA = FTOI16((px.A*0.5f)+0.5f);
			iB = FTOI16((px.B*0.5f)+0.5f);			
			
			
			// == Write back to image plane
			*(pOut)        = iL;
			*(pOut + pw)   = iA;
			*(pOut + 2*pw) = iB;

		}
	}
}
