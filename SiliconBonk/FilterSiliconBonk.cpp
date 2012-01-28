#include "FilterSiliconBonk.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"

#include "colorspace.h"
#include "blend_modes.h"
#include "fastmath.h"

#include "sqrtlut.h"

#include <QDebug>
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif


//#include "rgb_to_hsv.h"
#define FILTERNAME SiliconBonkFilter

#define FASTLOCAL static inline
//#define FASTLOCAL 

using namespace std;

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

//redefine here for faster versions

#define SIN fastsin
#define COS fastcos

/*float FASTLOCAL ContrastBernd(const float val, const float c) {
    return val-((c+0.5f)/4.4f-0.1f)*SIN(2*M_PI*val);
}
*/
float FASTLOCAL LinearContrast(const float val, const float c) {
    return ((val-0.5f)*c)+0.5f; //linear contrast
}

// needs precomputed factor
float FASTLOCAL ContrastBerndFast(const float val, const float c) {
    return val-c*SIN(2*M_PI*val);
}

float FASTLOCAL precomputeContrastFactor(const float c) {
    return ((c/*+0.5f*/)/4.4f/*-0.1f*/);
}

float FASTLOCAL MidPoint(const float val, const float strength) {
//    return val + (  -strength*(val*val)+strength*val);
    return val + (  strength*(val-(val*val)));

}

//parameter setting for luminance to internal value
static inline float paramLtoValue(const float v) {
    return 1.f+0.5f*(v/50.f);
}


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
	
	uint16 iR,iG,iB;
	bool okay;
	float optL, optH,optContrast,optMid,optSat, optLinearContrast;
	bool optHighlights;
	const int numBands = 6;
	int optEQ[numBands];

	
	// ==== Read Options ======================================================
	
	optHighlights = layerOptions.getBool(ProtectHighlights, m_groupId, okay);
	
	
	optH = layerOptions.getInt(H, m_groupId, okay)/360.f;
	optL = layerOptions.getInt(L, m_groupId, okay);
    float optLmod = paramLtoValue(optL);
    
    
    //read contrast, slit into sinusoidal and linear contrast
	optContrast = layerOptions.getInt(Contrast, m_groupId, okay)/100.f;
	if(optContrast >1.f) {
	    optLinearContrast = 1.f+(2.f*(optContrast-1.f)*(optContrast-1.f)); //-1 to get everything > 1, +1 as +1 is equivalent to 0 (do nothing) x2.. boost, square to get a move even contrast change
	    optContrast = 1.f;
	} else {
    	optLinearContrast = (1.f+0.f);
    }
	
	
	optContrast = precomputeContrastFactor(optContrast);//precompute

	optMid = layerOptions.getInt(Mid, m_groupId, okay)/100.f;
	optSat = layerOptions.getInt(Sat, m_groupId, okay)/100.f;
	
	optEQ[0] = layerOptions.getInt(C0L, m_groupId, okay);
	optEQ[1] = layerOptions.getInt(C1L, m_groupId, okay);
	optEQ[2] = layerOptions.getInt(C2L, m_groupId, okay);
	optEQ[3] = layerOptions.getInt(C3L, m_groupId, okay);
	optEQ[4] = layerOptions.getInt(C4L, m_groupId, okay);
	optEQ[5] = layerOptions.getInt(C5L, m_groupId, okay);
	
	float optEQmod[numBands];
	bool optEQenabled = false;
	for(int i = 0;i<numBands;i++) {
	    optEQmod[i] = paramLtoValue(optEQ[i]);
	    if(optEQ[i] != 0) optEQenabled = true; //check if at least one band is enabled
	}
    
   // if(!optHighlights) optMid += 0.1f; //increment mids if highlight protection is off
	// ========================================================================
    rgb_color rgb;
	
	for ( int y = 0; y < tileHeight; y++ ) {
		//	image-data16() points to the very first unpadded pixel
		//	add to that (rw * y) to point us at the first unpadded pixel for line 'y'
		pSrc = pSrcImg->data16() + rw * y;
		pOut = pDstImg->data16() + rw * y;
		
		// ======== Per Pixel Loop ============================================
		for ( int x = 0; x < tileWidth ; x++, pOut += np, pSrc += np ) { //	move to the next pixel (in the color plane)
		    // == read from image plane
			iR = *(pSrc)        ;
			iG = *(pSrc + pw)   ;
			iB = *(pSrc + 2*pw) ;

            //Use LUT lookup, int to float and gamma2 at the same time
            rgb.r = sqrtLUT[iR];
            rgb.g = sqrtLUT[iG];
            rgb.b = sqrtLUT[iB];
            
            hsv_color hsv = RGB2HCLnew(rgb);                            //to hsv

            float alpha = (COS((hsv.hue-optH)*(2.f*M_PI)))*.5f;                 //cosinal hue difference
            alpha *= clipf(hsv.sat*(2.f-hsv.sat),0.f,1.f);                                     //effect scales with saturation, quadratic curve
            float valMod = optLmod*hsv.val;                                     //modified liminance value
            float valNew = BLEND(valMod,hsv.val,alpha);                         //blend depending on alpha
            
            //===== TEST: band based modification =======
            if(optEQenabled) {
                float band      = hsv.hue*numBands;             //basic band
                int   loBand    = band;                         //lower band bin
                int   hiBand    = (loBand+1)%numBands;          //upper band bin
                float bandFract = band-loBand;                  //fractional part
                float newL      = optEQmod[loBand]*(1.f-bandFract) + optEQmod[hiBand]*bandFract;
                float alphaEQ   = clipf(hsv.sat*(2.f-hsv.sat),0.f,1.f);        //effect scales with saturation, quadratic curve
                float valModEQ    = newL*valNew;                //modified liminance value
                valNew    = BLEND(valModEQ,valNew,alphaEQ);     //blend depending on alpha
                //hsv.sat= clipf(BLEND(newL*hsv.sat,hsv.sat,alphaEQ),0.f,1.f);
       
            }
            //===== END TEST ============================
            
                               //blend depending on alpha
            
            //if(optHighlights) valNew = (-0.2782f*valNew+1.191f)*valNew;                           //prevent highlight blowout. quickly fitted in matalb
            //x = 0    0.1000    0.2000    0.3000    0.4000    0.5000    0.6000    0.7000    0.8000    0.9000    1.0000
            //yn = 0.0400    0.1200    0.2100    0.3050    0.4020    0.5010    0.6100    0.6900    0.7700    0.8400    0.9100
            if(optHighlights) valNew = (((-0.4481f*valNew)+0.5489f)*valNew+0.7659f)*valNew+0.03891f;   //prevent highlight blowout. quickly fitted in matalb
            
            
            valNew = MidPoint(valNew,optMid);                                   //midpoint adjustment
             
            valNew = ContrastBerndFast(clipf(valNew,0.f,1.f),optContrast);                     //contrast (bernds method
            valNew = LinearContrast(valNew,optLinearContrast); //linear contrast
            
            hsv.val = valNew;                                                   //apply and set colors to 0
            hsv.sat *= optSat;
            hsv.val = clipf(hsv.val,0.f,1.f);
            rgb = HCLnew2RGB(hsv);
            
            rgb.r = clipf(rgb.r,0.f,1.f);
            rgb.g = clipf(rgb.g,0.f,1.f);
            rgb.b = clipf(rgb.b,0.f,1.f);
            
            rgb.r *= rgb.r;
            rgb.g *= rgb.g;
            rgb.b *= rgb.b;
            
			//back to int
			iR = FTOI16(rgb.r);
			iG = FTOI16(rgb.g);
			iB = FTOI16(rgb.b);
			
			// == Write back to image plane
			*(pOut)        = iR;
			*(pOut + pw)   = iG;
			*(pOut + 2*pw) = iB;
		} // ======== End Pixel Loop ==========================================
	}
}
