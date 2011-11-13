/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
#ifndef DENOISE_PLUGIN_H
#define DENOISE_PLUGIN_H

#include "sphLayerFilter.h"

class DenoiseFilter : public sphLayerFilter
{
public:
	DenoiseFilter(PluginHub *hub, int groupId) :sphLayerFilter(hub,groupId,RGB2LAB,false){ } ;
	//virtual ~DenoiseFilter() { ; }

	enum OptionNames { Enable = 1, TresL=2,LowL=3,TresC=4,LowC=5,DePepper=8,ShowEdges=9};
	virtual QString name() const { return QString("WaveletDenoise2Filter"); }

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings &settings, PluginOptionList  &layerOptions) const;
	virtual bool isSlow(const ImageSettings &options, const PipeSettings &settings) const;
	virtual bool needsOriginalImage  ();

	virtual int additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const;
	virtual void ProcessBuffer(float *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const;
};


#endif
