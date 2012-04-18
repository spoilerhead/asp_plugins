#ifndef AngyPlugin_H
#define AngyPlugin_H

#include "B5Plugin.h"
#include <QObject>
#include <QString>

#include "PluginImageSettings.h"
#include "PluginOptionList.h"

class AngyPlugin : public QObject, public B5Plugin
{
Q_OBJECT
public:
	AngyPlugin() { ; }
	virtual int priority() { return 120; }
	virtual QString name() { return QString("Angy"); }
	virtual QString group() { return QString("Angy"); }
	virtual bool init(PluginHub *hub, int id, int groupId, const QString &bundlePath);
	virtual bool registerFilters();
	virtual bool registerOptions();
	virtual bool finish();
	virtual PluginDependency *createDependency(const QString &name);
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
