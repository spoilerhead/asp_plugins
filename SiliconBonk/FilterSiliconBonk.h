#pragma once

#include "PluginLayerFilter.h"
#include "PluginHub.h"


#define LayerFilter PluginLayerFilter
#define ImageSettings PluginImageSettings
#define PipeSettings PluginPipeSettings
/*
#define LayerFilter PluginLayerFilter
#define ImageSettings PluginImageSettings
#define PipeSettings PluginPipeSettings
*/

class SiliconBonkFilter : public LayerFilter
{
public:
	SiliconBonkFilter(PluginHub *hub, int groupId) : m_hub(hub), m_groupId(groupId) { ; }
	
	virtual QString name() const { return QString("SiliconBonkFilter"); }
	
	enum OptionNames { Enable = 1, H=2,L=3,Contrast=4,Mid=5,Sat=6,ProtectHighlights=7,C0L=8,C1L=9,C2L=10,C3L=11,C4L=12,C5L=13};

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
