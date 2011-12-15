/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010-2011
 * licensed: GPL v2+, other licenses  available on request
 */
 
#ifndef sphlayerfilter_PLUGIN_H
#define sphlayerfilter_PLUGIN_H

#include "PluginOptionList.h"
#include "PluginLayerFilter.h"
#include "PluginHub.h"


#include "colorspace.h"
#include "blend_modes.h"
using namespace std;


//Define this to use float as working space, its actually just slightly slower, but quality is a lot better
#define USE_FLOAT_SPACE

#ifdef USE_FLOAT_SPACE
#define clip clipf
#define to_workspace I16TOF
#define from_workspace FTOI16
typedef float pixel;

#else

#define clip clipi
#define to_workspace I16TOI32
#define from_workspace I32TOI16
typedef int pixel;

#endif

//How many neighboring tiles does the plugin need? don't set it to 0!
#define neighborTiles 1
//TODO: rework

#define LayerFilter PluginLayerFilter
#define ImageSettings PluginImageSettings
#define PipeSettings PluginPipeSettings
/*
#define LayerFilter PluginLayerFilter
#define ImageSettings PluginImageSettings
#define PipeSettings PluginPipeSettings
*/


class sphLayerFilter : public LayerFilter
{
    
public:
    enum ConvertPixel {NONE, RGB2YCBCR, RGB2HSL, RGB2HSV, RGB2HCL,RGB2LAB, RGB2XYZ};
    enum DeGamma {GAMMA_NONE, GAMMA_FIRST, GAMMA_ALL};
	sphLayerFilter(PluginHub *hub, int groupId, const ConvertPixel conversion=NONE, const DeGamma deGamma=GAMMA_NONE) : m_hub(hub), m_groupId(groupId), m_conversion(conversion), m_deGamma(deGamma) { ; }
	//virtual ~UsmFilter() { ; }
	virtual QString name() const =0;

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const=0;
	virtual bool isSlow(const ImageSettings &options, const PipeSettings  &settings) const=0;
	virtual bool isLargeRadius(const ImageSettings &options, const PipeSettings  &settings) const;
	//virtual bool isPixelSource(const PluginImageSettings &options, const PluginPipeSettings &settings) const;
	//virtual QTransform transform(const PluginImageSettings &options, const PluginPipeSettings &settings) const;
	//virtual QSize size(const PluginImageSettings &options, const PluginPipeSettings &settings, const QSize &startSize) const;
	virtual QList<PluginDependency*> prerequisites(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile) const;
	virtual bool 	needsOriginalImage  ()=0;

	virtual void runLayer(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const;
	
	virtual int additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const=0;
	virtual void ProcessBuffer(pixel *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const =0;
protected:
    
    //calculate buffer rectangle for this tile
    QRect getBufferRect      (PluginOptionList  *layerOptions, const PipeSettings  &settings, PluginTile &tile) const;
    //same as above but intersected with the actual image
    QRect getPrerequisiteRect(PluginOptionList  *layerOptions, const PipeSettings  &settings, PluginTile &tile) const;
    

	PluginHub *m_hub;
	int m_groupId;
	ConvertPixel m_conversion;
	DeGamma m_deGamma;
};


#endif
