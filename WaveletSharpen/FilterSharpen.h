/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
#ifndef FILTERSHARPEN_H
#define FILTERSHARPEN_H

#include "sphLayerFilter.h"

class WaveletSharpenFilter : public sphLayerFilter
{
public:
	WaveletSharpenFilter(PluginHub *hub, int groupId) :sphLayerFilter(hub,groupId,RGB2LAB,GAMMA_NONE){ } ;
	//virtual ~SharpenFilter() { ; }

	enum OptionNames { Enable = 1, Amount = 2, Radius = 3, /*Blend = 4,*/ DePepper = 5, Edges=6, Clarity=7,
	                Enable2 = 11, Amount2 = 12, Radius2 = 13, /*Blend2 = 14,*/ DePepper2 = 15, Edges2=16, Clarity2=17
	                };
	virtual QString name() const { return QString("WaveletSharpen2_Wavelet_Filter"); }

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const;
	virtual bool isSlow(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual bool needsOriginalImage  ();

	virtual int additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const;
	virtual void ProcessBuffer(float *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const;
};


#endif
