/*
 * by Dieter Steiner <spoilerhead@gmail.com>
 * (C) 2012
 * licensed: GPL v2+
 */
#ifndef FILTERPYRAMID_H
#define FILTERPYRAMID_H

#include "sphLayerFilter.h"

class PyramidFilter : public sphLayerFilter
{
public:
	PyramidFilter(PluginHub *hub, int groupId) :sphLayerFilter(hub,groupId,RGB2LAB,GAMMA_NONE){ } ;
	//virtual ~SharpenFilter() { ; }

	enum OptionNames { 
	                EnablePyramid=1,P1=2,P2=3,P3=4,P4=5,P5=6,P6=7,P7=8,P8=9,P9=10,
	                Shadow=100,Mid=101,Light=102

	                };
	virtual QString name() const { return QString("aLiCE_Pyramid_Filter"); }

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList  &layerOptions) const;
	virtual bool isSlow(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual bool needsOriginalImage  ();

    virtual int getLastNonZeroPyramid(PluginOptionList  *layerOptions) const;
	virtual int additionalRadius(PluginOptionList  *layerOptions,float zoomLevel) const;
	virtual void ProcessBuffer(float *fimg[3],int width,int height,float zoomLevel,const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList  &layerOptions, int layerPos) const;
};


#endif
