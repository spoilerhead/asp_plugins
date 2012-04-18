#pragma once

#include "PluginLayerFilter.h"
#include "PluginHub.h"

#include <QComboBox>

#define LayerFilter PluginLayerFilter
#define ImageSettings PluginImageSettings
#define PipeSettings PluginPipeSettings
/*
#define LayerFilter PluginLayerFilter
#define ImageSettings PluginImageSettings
#define PipeSettings PluginPipeSettings
*/

class AngyFilter : public LayerFilter
{
public:
	AngyFilter(PluginHub *hub, int groupId) : m_hub(hub), m_groupId(groupId) { ; }
	
	virtual QString name() const { return QString("AngyFilter"); }
	
	enum OptionNames { Enable = 1, Bleach=2, Screen=4,Total=5,Flatten=6,Brightness=7,Recovery=8,Degamma=9};

	virtual bool needsToRunLayer(const ImageSettings &options, const PipeSettings  &settings, PluginOptionList &layerOptions) const;
	virtual void runLayer(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList &layerOptions, int layerPos) const;
	virtual bool needsOriginalImage() { return false; };
	
	virtual bool isSlow(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual bool isLargeRadius(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual bool isPixelSource(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual QTransform transform(const ImageSettings &options, const PipeSettings  &settings) const;
	virtual QSize size(const ImageSettings &options, const PipeSettings  &settings, const QSize &startSize) const;
	virtual QList<PluginDependency*> prerequisites(const ImageSettings &options, const PipeSettings  &settings, PluginTile &tile) const;

private:
	PluginHub *m_hub;
	int m_groupId;
};
