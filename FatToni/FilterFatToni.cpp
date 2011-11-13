#include "FilterFatToni.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"

#include "colorspace.h"
#include "blend_modes.h"
#include "fastmath.h"

#include <QDebug>
#include <iostream>
#include <cmath>




//#include "rgb_to_hsv.h"
#define FILTERNAME FatToniFilter

#define FASTLOCAL static inline
//#define FASTLOCAL 

using namespace std;

//================================================================================

bool FILTERNAME::needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList &layerOptions) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	bool okay;
	bool val = layerOptions.getBool(FILTERNAME::Enable, m_groupId, okay);
	return val;
}

bool FILTERNAME::isSlow(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;
}

bool FILTERNAME::isLargeRadius(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;	//	We only use the current tile.
}

bool FILTERNAME::isPixelSource(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return false;	//	We don't generate pixels from scratch.
}

QTransform FILTERNAME::transform(const ImageSettings &options, const PipeSettings  &settings) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return QTransform();	//	We don't warp the image so return an identity transform.
}

QSize FILTERNAME::size(const ImageSettings &options, const PipeSettings  &settings, const QSize &startSize) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	
	return startSize;	//	We don't change the size of the image.
}

QList<PluginDependency*> FILTERNAME::prerequisites(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile) const
{
	Q_UNUSED( options );
	Q_UNUSED( settings );
	Q_UNUSED( tile );
	
	return QList<PluginDependency*>();	//	Empty list - we don't depend on any other tiles.
}


//redefine here for faster versions (see fastmath.h)

#define SIN fastsin
#define COS fastcos
#define ATAN2 fastatan2
#define SQRT sqrt

// needs precomputed factor
float FASTLOCAL ContrastBerndFast(const float val, const float c) {
    return val-c*SIN(2*M_PI*val);
}

void FILTERNAME::runLayer(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList &layerOptions, int layerPos) const
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
	
	
	// ==== Read Options ======================================================
	
	float optHH = layerOptions.getInt(HH, m_groupId, okay)/360.f;
	float optSH = layerOptions.getInt(SH, m_groupId, okay)/100.f;
	float optHS = layerOptions.getInt(HS, m_groupId, okay)/360.f;
	float optSS = layerOptions.getInt(SS, m_groupId, okay)/100.f;	
	float optCont = layerOptions.getInt(Cont, m_groupId, okay)/100.f;
	float optMid = layerOptions.getInt(Mid, m_groupId, okay)/200.f;
	float optMix = layerOptions.getInt(Mix, m_groupId, okay)/100.f;
	
	float optBaseH = layerOptions.getInt(BaseH, m_groupId, okay)/360.f;
	float optBaseS = layerOptions.getInt(BaseS, m_groupId, okay)/100.f;
	float optBaseL = layerOptions.getInt(BaseL, m_groupId, okay)/100.f;
	
	
	//DEFINE COLORS==
    //shadows
    //float xs = optSS*COS(optHS*(2*M_PI));
    //float ys = optSS*SIN(optHS*(2*M_PI));
    
    //highlights
    //float xh = optSH*COS(optHH*(2*M_PI));
    //float yh = optSH*SIN(optHH*(2*M_PI));
    
    
    hsv_color highlight;
    highlight.sat   = optSH;
    highlight.hue   = optHH;
    highlight.val = 0.5f;


    hsv_color shadow;
    shadow.sat      = optSS;    
    shadow.hue      = optHS;
    shadow.val = 0.5f;
    
    hsv_color base;
    base.sat    = optBaseS;
    base.hue    = optBaseH;
    base.val    = optBaseL;
 
    
    rgb_color srgb = HSL2RGB(shadow);
    rgb_color hrgb = HSL2RGB(highlight);
    rgb_color basergb = HSL2RGB(base);


//    qDebug()<<optHH <<""<<optSH <<""<<optHS <<""<<optSS <<"";
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
            rgb.r = fastsqrt(I16TOF(iR));                                                 //to float space
            rgb.g = fastsqrt(I16TOF(iG));
            rgb.b = fastsqrt(I16TOF(iB));
            
            float luma = (rgb.r+rgb.g+rgb.b)/3.f;//HCL_VAL(rgb); //pixel luminance
			
			
			float toneAlpha;// = luma;//(luma<0.5) ? ((luma)/(2*BLENDPOINT)) : (0.5f+(luma-BLENDPOINT)/(2*(1-BLENDPOINT)));
			toneAlpha = clipf(luma+(optMid),0.f,1.f);
			
			toneAlpha = clipf(ContrastBerndFast(toneAlpha,0.5f*optMix),0.f,1.f);
			//toneAlpha = clipf(  (((toneAlpha-0.5f)*(1.f+3.f*optMix))+0.5f),0.f,1.f);

            
            //toning color for this pixel
			rgb_color toning;
            toning.r = BLEND( hrgb.r, srgb.r, toneAlpha);
			toning.g = BLEND( hrgb.g, srgb.g, toneAlpha);
			toning.b = BLEND( hrgb.b, srgb.b, toneAlpha);

			//Mix back a bit of b+w
			/*toning.r = BLEND(toning.r, rgb.r, 0.75f+(0.25f*optCont));
			toning.g = BLEND(toning.g, rgb.g, 0.75f+(0.25f*optCont));
			toning.b = BLEND(toning.b, rgb.b, 0.75f+(0.25f*optCont));
*/


			
			rgb.r = BLEND ( OVERLAY( toning.r ,rgb.r), MIX( toning.r ,rgb.r),   optCont);
			rgb.g = BLEND ( OVERLAY( toning.g ,rgb.g), MIX( toning.g ,rgb.g),   optCont);
			rgb.b = BLEND ( OVERLAY( toning.b ,rgb.b), MIX( toning.b ,rgb.b),   optCont);
	
			
            rgb.r = MULTIPLY(basergb.r, rgb.r);
			rgb.g = MULTIPLY(basergb.g, rgb.g);
			rgb.b = MULTIPLY(basergb.b, rgb.b);
			
			
			
			iR = FTOI16(rgb.r*rgb.r);
			iG = FTOI16(rgb.g*rgb.g);
			iB = FTOI16(rgb.b*rgb.b);
			
			// == Write back to image plane
			*(pOut)        = iR;
			*(pOut + pw)   = iG;
			*(pOut + 2*pw) = iB;
		} // ======== End Pixel Loop ==========================================
	}
}
