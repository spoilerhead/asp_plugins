/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */

#include "GrainFilter.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"
#include "PluginImageSettings.h"
#include "PluginOptionList.h"
#include <QDebug>

#include <iostream>
#include <cmath>
//Hacked up by Spoilerhead

//#include "usm_iir.h"
#include "gauss_iir.h"

using namespace std;

#define FilterName GrainFilter

#define PIXELBASED_SIZE 1

bool FilterName::needsToRunLayer(const ImageSettings &options, const PipeSettings &settings, PluginOptionList  &layerOptions) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	bool okay;
	bool val = layerOptions.getBool(Enable, m_groupId, okay);
	
	
/*	bool randomSeed = layerOptions.getBool(GrainFilter::Random, m_groupId,okay);
	//qDebug()<<randomSeed<<" "<<okay;
	if(okay && randomSeed) {
	    int newSeed = (rand()%256);
	    //qDebug()<<"Grain filter, Random Seed Selected: "<<newSeed;
    	layerOptions.setInt(GrainFilter::Seed, m_groupId, newSeed);
    }*/
    

	
	return val;
}

bool FilterName::isSlow(const ImageSettings &options, const PipeSettings &settings) const
{
    Q_UNUSED(options);
    Q_UNUSED(settings);
	return true;
}


bool FilterName::needsOriginalImage  ()
{
    return false;
}

/** how many pixels outside the padding does the plugin need to work? (in each direction)
*/
int FilterName::additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const
{
    bool okay;
    float radius =layerOptions->getInt(Radius, m_groupId, okay)*zoomLevel/10.0f;///10.0f; //= 0.5; //0..2
    //use this place to generate a random Seed
    int rndSeed = layerOptions->getInt(HiddenSeed, m_groupId,  okay );
    if(rndSeed == 0) layerOptions->setInt(HiddenSeed, m_groupId,  (rand()%65535)+1);
    
    return radius*2.f;
}


inline unsigned int qhash( unsigned int a) {
    a = (a ^ 61) ^ (a >> 16);
    a = a + (a << 3);
    a = a ^ (a >> 4);
    a = a * 0x27d4eb2d;
    a = a ^ (a >> 15);
    return a;
}

inline unsigned int hash32shift(unsigned int key)
{
  key = ~key + (key << 15); // key = (key << 15) - key - 1;
  key = key ^ (key >> 12);
  key = key + (key << 2);
  key = key ^ (key >> 4);
  key = key * 2057; // key = (key + (key << 3)) + (key << 11);
  key = key ^ (key >> 16);
  return key;
}

inline unsigned int jenkins_hash( unsigned int a)
{
   a = (a+0x7ed55d16) + (a<<12);
   a = (a^0xc761c23c) ^ (a>>19);
   a = (a+0x165667b1) + (a<<5);
   a = (a+0xd3a2646c) ^ (a<<9);
   a = (a+0xfd7046c5) + (a<<3);
   a = (a^0xb55a4f09) ^ (a>>16);
   return a;
}

inline int mix(int a, int b, int c)
{
  a=a-b;  a=a-c;  a=a^(c >> 13);
  b=b-c;  b=b-a;  b=b^(a << 8); 
  c=c-a;  c=c-b;  c=c^(b >> 13);
  a=a-b;  a=a-c;  a=a^(c >> 12);
  b=b-c;  b=b-a;  b=b^(a << 16);
  c=c-a;  c=c-b;  c=c^(b >> 5);
  a=a-b;  a=a-c;  a=a^(c >> 3);
  b=b-c;  b=b-a;  b=b^(a << 10);
  c=c-a;  c=c-b;  c=c^(b >> 15);
  return c;
}

inline static unsigned int MurmurHash2 ( const unsigned int key, const unsigned int seed ) {
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value
    unsigned int h = seed;
    unsigned int k = key*m;
    k ^= k >> r; 
    k *= m; 
		
    h *= m; 
    h ^= k;
   
    h ^= h >> 13;
	h *= m;
	h ^= h >> 15;
	
	return h;
}


inline unsigned int mNoise(const unsigned int x, const unsigned int y, const unsigned int c, const unsigned int seed) {
//mix(ix,iy,c+jenkins_hash(id+((seed)<<iter))); 
    //unsigned int n = (x^2504478101);//^(y*0xf136bc83)^(c*60013)^(seed*409);
    //n ^= n<<17;
    //return n;//(n * (n * n * 15731 + 789221) + 1376312589) ;  
    return MurmurHash2((x<<16^y)  ,seed^c);
    
}

/** Process here, Pixels get passed as 3 arrays of width*height, split by channels
*/
void FilterName::ProcessBuffer(pixel *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const
{
        Q_UNUSED(options);
        Q_UNUSED(settings);
        Q_UNUSED(tile);
        Q_UNUSED(layerPos);
        
   
        
        
        //Read options
        bool  okay;
        #ifndef PIXELBASED_SIZE
        float radius =layerOptions.getInt(Radius, m_groupId, okay)/20000.0f; //size in percents of the image width now
        #endif
        #ifdef PIXELBASED_SIZE
        float radius =layerOptions.getInt(Radius, m_groupId, okay)*zoomLevel/10.0f;//size in pixels
        #endif
        
    	float amount = layerOptions.getInt(Amount, m_groupId, okay)*zoomLevel /**zoomLevel*/ /100.0f*2.0f;/*zoomLevel*/
    	int   seed = layerOptions.getInt(Seed, m_groupId, okay);
    	
    	bool randomSeed = layerOptions.getBool(Random, m_groupId, okay); //random seed?
    	if(randomSeed) {
    	    seed = layerOptions.getInt(HiddenSeed, m_groupId, okay);
    	    //qDebug()<<"Using random Seed";
    	}
        
        bool colorGrain = layerOptions.getBool(ColorGrain, m_groupId, okay); //color grain?
    	
    	//generate parabolic parameters
    	float shadow = layerOptions.getInt(Shadow, m_groupId, okay)/100.0f;;
    	float mid    = layerOptions.getInt(Mid, m_groupId, okay)/100.0f;;
    	float lights = layerOptions.getInt(Light, m_groupId, okay)/100.0f;;
    	const float denom = -0.25f;//(0.0f - 0.5f) * (0.0f - 1.0f) * (0.5f - 1.0f);
        float A     = (1.0f * (mid - shadow) + 0.5f * (shadow - lights) + 0.0f * (lights - mid)) / denom;
        float B     = (1.0f*1.0f * (shadow - mid) + 0.5f*0.5f * (lights - shadow) + 0.0f*0.0f * (mid - lights)) / denom;
        float C     = (0.5f * 1.0f * (0.5f - 1.0f) * shadow + 1.0f * 0.0f * (1.0f - 0.0f) * mid + 0.0f * 0.5f * (0.0f - 0.5f) * lights) / denom;


        //Get offsets for pixels
    	int tileWidth  = settings.tileSize().width();//+2*padw;        //Tile Dimensions
        int tileHeight = settings.tileSize().height();//+2*padh;
        int padX = (width-tileWidth)>>1;
        int padY = (height-tileHeight)>>1;

        #ifndef PIXELBASED_SIZE
    	int srcStep = m_hub->step(this) - 1;
    	radius = settings.size(srcStep).width()*radius; //Radius in pixels now;
    	#endif
    	
        int tileX = tile.x();
        int tileY = tile.y();
    	
    	
    	
    	int grainChannels = 1;
    	
    	static const float channelGain[3] ={1.0f,0.25f,0.1666f};
    	const int iterations[3] = {3,1,1};                                     //we perform multiple iterations, to approximate gaussian noise, more iterations give a sharper distribution
    	
    	if( colorGrain ) {
    	    grainChannels = 3;      //create RGB grain
    	} else {                    //create luma grain
    	    grainChannels = 1;
        	
    	}
  	        
        //Grain field buffer
        pixel *grainField = new pixel[width*height];
    	
    	for(int c = grainChannels-1;c>=0;c--) {
        	
        	
        	//Generate Grain field
        	for(int i = 0; i<(width*height); i++) {
              int ix = ((i%width)-padX)+tileX;  // absolute X coordinate of the pixel
              int iy = ((i/width)-padY)+tileY;  // absolute Y coordinate
              unsigned int id = ix+(iy<<16);    //pixel id

              float grain = 0.0f;  //initialize
             

              for(int iter = 0; iter <iterations[c]; iter++ ) {
                //unsigned int qh = mix(ix,iy,c+jenkins_hash(id+((seed)<<iter)));   //hash the pixel TODO: find a faster method
                unsigned int qh = mNoise(ix,iy,c,seed<<iter);
                grain += ((qh /4294967295.0f )-0.5f);                       //pseudorandom number -0.5..0.5
              }
              grain /= iterations[c];
              grain *=amount*(radius+1.0f);                                //boost strength
              grain *=channelGain[c]; //channel specific gain
              grain += 0.50f;                                               //move back to midpoint 0.5
              
              //fimg[0][i]   = clip(grain,0.0f,1.0f);
              grainField[i] = grain;
            }
            
        	//Blur the Grain
            GAUSS_IIR(grainField,width,height,radius);

           
             //Blend the grain in
            for(int i = 0;i<(width*height);i++) {
              float blendlevel = clip((A*(fimg[0][i]*fimg[0][i])+B*fimg[0][i]+C),0.0f,1.0f);  //calculate alpha of the grainy image, based on the luminance
              fimg[c][i] = fimg[c][i] + (grainField[i] -0.5f)*blendlevel;                //simply ADD the grain, this works better than overlay
              fimg[c][i] = clip(fimg[c][i],0.0f,1.0f);                            //Clip
            }
        
        
        } //end per channel threatment
        
        delete[] grainField;  //Free Buffer
      
}



