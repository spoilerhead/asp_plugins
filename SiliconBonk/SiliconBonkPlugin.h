#ifndef SiliconBonkPLUGIN_H
#define SiliconBonkPLUGIN_H

#include "B5Plugin.h"
#include <QObject>
#include <QString>

#include "PluginImageSettings.h"
#include "PluginOptionList.h"

class SiliconBonkPlugin : public QObject, public B5Plugin
{
Q_OBJECT
public:
	SiliconBonkPlugin() { ; }
	virtual int priority() { return 110; }
	virtual QString name() { return QString("SiliconBonk"); }
	virtual QString group() { return QString("SiliconBonk"); }
	virtual bool init(PluginHub *hub, int id, int groupId, const QString &bundlePath);
	virtual bool registerFilters();
	virtual bool registerOptions();
	virtual bool finish();
	virtual PluginDependency *createDependency(const QString &dname);
    virtual QList<QString> toolFiles();
    virtual QList<QWidget*> toolWidgets();
    virtual void toolWidgetCreated(QWidget *uiWidget);
    
    

    
public slots:  
    void handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer);
    void handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes);
    void handleHotnessChanged(const PluginImageSettings &options);
    void reset();

private:
    
	PluginHub *m_hub;
	int m_id;
	int m_groupId;
};



#endif
