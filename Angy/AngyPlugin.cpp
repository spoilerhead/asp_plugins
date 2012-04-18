#include "AngyPlugin.h"
#include "FilterAngy.h"


#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>


#include <QWidget>
#include <QPushButton>
#include <QSettings>


#define PLUGIN_NAME_HR "Angy"


extern "C" BIBBLE_API B5Plugin *b5plugin() { return new AngyPlugin; }


bool AngyPlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
    qDebug() << "Initialising "<<PLUGIN_NAME_HR<<" filter";
    
    if(hub == NULL) return false;
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	
	// Store our group ID and group name (andrewj)
//    QSettings oSettings("Bibble and AfterShot Plugins", "PluginGroups");
//    oSettings.setValue(group(), groupId);
	return true;
}

bool AngyPlugin::registerFilters()
{
    AngyFilter *angy = new AngyFilter(m_hub, m_groupId);
    
    
    if (m_hub->addFilter(angy, PluginHub::Before, QString("Curves"))) {
        qDebug() << "Successfully registered "<<angy->name()<<" filter after Curves";
	} else {
    	qDebug() << "Failed to register "<<angy->name()<<" filter";
	    return false;
	}

	return true;
}

#define SNONE "None"
bool AngyPlugin::registerOptions()
{
	m_hub->addBoolOption  (AngyFilter::Enable   ,   "AngyOn" ,      "Enabled"           , tr("Enable Plugin"), "AngyFilter", false, false, 0);
	m_hub->addBoolOption  (AngyFilter::Bleach   ,   "AngyBleach" ,  "Bleach"        , tr(""), "AngyFilter", false, false, 0);
	
	m_hub->addIntOption   (AngyFilter::Screen   ,   "AngyOpa" ,     ""           , tr(""), "AngyFilter", 80, 0, 0);
	m_hub->addIntOption   (AngyFilter::Total    ,   "AngyStrength", ""           , tr(""), "AngyFilter", 100, 0, 0);
	m_hub->addIntOption   (AngyFilter::Flatten  ,   "AngyEdge" ,    ""           , tr(""), "AngyFilter", 0, 0, 0);
	m_hub->addIntOption   (AngyFilter::Brightness,  "AngyBrightness",""           , tr(""), "AngyFilter", 0, 0, 0);
	m_hub->addIntOption   (AngyFilter::Recovery ,   "AngyRecovery", ""           , tr(""), "AngyFilter", 0, 0, 0);
	m_hub->addIntOption   (AngyFilter::Degamma  ,   "AngyDegamma",  ""           , tr(""), "AngyFilter", 100, 0, 0);

	return true;
}

bool AngyPlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *AngyPlugin::createDependency(const QString &name)
{
	Q_UNUSED(name);
	return NULL;
}

QList<QString> AngyPlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> AngyPlugin::toolWidgets()
{
	return QList<QWidget*>();
}

void AngyPlugin::toolWidgetCreated(QWidget *uiWidget)
{
    //Q_UNUSED(uiWidget);
    if(uiWidget == NULL) return; //protection
    
    
    /*if (QPushButton *rbtn = uiWidget->findChild<QPushButton*>("FatToniReset_btn")) {
			connect(rbtn, SIGNAL(clicked()), this, SLOT(reset()));
		}
*/
}

//========================================================================================================
void AngyPlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{

    Q_UNUSED(options);
    Q_UNUSED(changed);
    Q_UNUSED(currentLayer);
}

void AngyPlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
{

    Q_UNUSED(optionName);
    Q_UNUSED(groupId);
    Q_UNUSED(current);
    //   v-- You need to add this if you're going to use the options --v
    //   some changes do not have options - check for NULL
    //    qDebug()<<"handlecontrolchange_prenull";
    if (options.options(layer) == NULL) return;
    //bool okay;
    
    //qDebug()<<"handlecontrolchange";
    // USM settings changes   
/*	if(
	    changes.contains(AngyFilter::HH, m_groupId)
	  ||changes.contains(AngyFilter::SH, m_groupId)
	  ||changes.contains(AngyFilter::HS, m_groupId)
	  ||changes.contains(AngyFilter::SS, m_groupId)
	  ||changes.contains(AngyFilter::Cont, m_groupId)
	  ||changes.contains(AngyFilter::Mid, m_groupId)
	  ||changes.contains(AngyFilter::Mix, m_groupId)
	  ||changes.contains(AngyFilter::BaseH, m_groupId)
	  ||changes.contains(AngyFilter::BaseS, m_groupId)
	  ||changes.contains(AngyFilter::BaseL, m_groupId)
	){
        changes.setBool(AngyFilter::Enable, m_groupId, true);
	}
	*/
	
}

//========================================================================================================
void AngyPlugin::handleHotnessChanged(const PluginImageSettings &options)
{

    Q_UNUSED(options);
}

void AngyPlugin::reset() {
   /* if (PluginOptionList *options = m_hub->beginSettingsChange("Reset Fat Toni")) {
		options->setInt(AngyFilter::HH, m_groupId, DEFAULT_HH);
		options->setInt(AngyFilter::SH, m_groupId, DEFAULT_SH);
		options->setInt(AngyFilter::HS, m_groupId, DEFAULT_HS);
		options->setInt(AngyFilter::SS, m_groupId, DEFAULT_SS);
		options->setInt(AngyFilter::Mid, m_groupId, DEFAULT_MID);
		options->setInt(AngyFilter::Mix, m_groupId, DEFAULT_MIX);
		options->setInt(AngyFilter::Cont, m_groupId, DEFAULT_CONT);
		options->setInt(AngyFilter::BaseH, m_groupId, DEFAULT_BASEH);
		options->setInt(AngyFilter::BaseS, m_groupId, DEFAULT_BASES);
		options->setInt(AngyFilter::BaseL, m_groupId, DEFAULT_BASEL);
		m_hub->endSettingChange();
	}*/
}
