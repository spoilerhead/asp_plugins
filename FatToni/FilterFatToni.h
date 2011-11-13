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

class FatToniFilter : public LayerFilter
{
public:
	FatToniFilter(PluginHub *hub, int groupId) : m_hub(hub), m_groupId(groupId) { ; }
	
	virtual QString name() const { return QString("FatToniFilter"); }
	
	enum OptionNames { Enable = 1, HH=2,SH=3,HS=4,SS=5,Cont=6,Mid=7,Mix=8,BaseH=9,BaseS=10,BaseL=11};

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
