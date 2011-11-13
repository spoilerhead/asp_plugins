/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
 
#ifndef Usm_PLUGIN_H
#define Usm_PLUGIN_H

#include "sphLayerFilter.h"

class GrainFilter : public sphLayerFilter
{
public:
	GrainFilter(PluginHub *hub, int groupId) :sphLayerFilter(hub,groupId,RGB2YCBCR,true){} ;
	//virtual ~UsmFilter() { ; }

	enum OptionNames { Enable = 1, Radius = 2, Seed = 3, Amount=4, Shadow=5,Mid=6,Light=7,Random=8,HiddenSeed=9,ColorGrain=10};
	virtual QString name() const { return QString("GrainFilter"); }

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings &settings, PluginOptionList  &layerOptions) const;
	virtual bool isSlow(const ImageSettings &options, const PipeSettings &settings) const;
	virtual bool needsOriginalImage  ();

	virtual int additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const;
	virtual void ProcessBuffer(float *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const;
//private:
    	
};


#endif
