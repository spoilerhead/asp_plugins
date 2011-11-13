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

int getFilmID(const QString &name);
int getColorFilterID(const QString &name);
int getDeveloperID(const QString &name);
int getPaperID(const QString &name);

void fillColorFiltersCombo(QComboBox *p_combo);
void fillFilmStockCombo(QComboBox *p_combo);
void fillDeveloperCombo(QComboBox *p_combo);
void fillPaperCombo(QComboBox *m_combo);

class GelatinFilter : public LayerFilter
{
public:
	GelatinFilter(PluginHub *hub, int groupId) : m_hub(hub), m_groupId(groupId) { ; }
	
	virtual QString name() const { return QString("GelatinFilter"); }
	
	enum OptionNames { Enable = 1, Color=2,EXPFilm=3,EXPPaper=4,FilmStock=5,IntensityBoost=6,BaseTint=7,ColorFilter=8,TintBoost=9,ColorShift=10,
	Developer=11,Paper=12,StretchContrast=13,Colorbleed=14/*,FilmStockGUI=15,ColorFilterGUI=16,DeveloperGUI=17,PaperGUI=18*/};

	virtual bool needsToRunLayer(const ImageSettings  &options, const PipeSettings  &settings, PluginOptionList &layerOptions) const;
	virtual void runLayer(const ImageSettings  &options, const PipeSettings  &settings, PluginTile &tile, PluginOptionList &layerOptions, int layerPos) const;
	virtual bool needsOriginalImage() { return false; };
	
	virtual bool isSlow(const ImageSettings  &options, const PipeSettings  &settings) const;
	virtual bool isLargeRadius(const ImageSettings  &options, const PipeSettings  &settings) const;
	virtual bool isPixelSource(const ImageSettings  &options, const PipeSettings  &settings) const;
	virtual QTransform transform(const ImageSettings  &options, const PipeSettings  &settings) const;
	virtual QSize size(const ImageSettings  &options, const PipeSettings  &settings, const QSize &startSize) const;
	virtual QList<PluginDependency*> prerequisites(const ImageSettings  &options, const PipeSettings  &settings, PluginTile &tile) const;

private:
	PluginHub *m_hub;
	int m_groupId;
};
