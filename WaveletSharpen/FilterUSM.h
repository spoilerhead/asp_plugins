/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2010
 * licensed: GPL v2+
 */
#ifndef FILTERUSM_H
#define FILTERUSM_H

#include "sphLayerFilter.h"

class USMFilter : public sphLayerFilter
{
public:
	USMFilter(PluginHub *hub, int groupId) :sphLayerFilter(hub,groupId,RGB2LAB,false){ } ;
	//virtual ~SharpenFilter() { ; }

	enum OptionNames { 
	                EnableUSM=21,RadiusUSM=22,AmountUSM=23,ThresholdUSM=24,USMasClarity=25,

	                };
	virtual QString name() const { return QString("WaveletSharpen2_USM_Filter"); }

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const;
	virtual bool isSlow(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual bool needsOriginalImage  ();

	virtual int additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const;
	virtual void ProcessBuffer(float *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const;
};


#endif
