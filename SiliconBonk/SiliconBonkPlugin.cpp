#include "SiliconBonkPlugin.h"
#include "FilterSiliconBonk.h"


#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>


#include <QWidget>
#include <QPushButton>
#include <QSettings>

#include "ToolData.h"

#define PLUGIN_NAME_HR "SiliconBonk"

#define DEFAULT_H   0
#define DEFAULT_L   0
#define DEFAULT_CONTRAST    0
#define DEFAULT_MID 0
#define DEFAULT_SAT 0
#define DEFAULT_HIGHLIGHT true
#define DEFAULT_EQUALIZER 0



extern "C" BIBBLE_API B5Plugin *b5plugin() { return new SiliconBonkPlugin; }


bool SiliconBonkPlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
    
    if(hub == NULL) return false;
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	
    // Store our group ID and group name (andrewj)
    QSettings oSettings("Bibble and AfterShot Plugins", "PluginGroups");
    oSettings.setValue(group(), groupId);
	return true;
}

bool SiliconBonkPlugin::registerFilters()
{
    SiliconBonkFilter *fBernd = new SiliconBonkFilter(m_hub, m_groupId);
    
   /* if (m_hub->addFilter(fBernd, PluginHub::Before, QString("Curves"))) {
        qDebug() << "Successfully registered "<<fBernd->name()<<" filter before Curves";
	} else*/
    
    
    if (m_hub->addFilter(fBernd, PluginHub::Before, QString("GrainFilter"))) {
        qDebug() << "Successfully registered "<<fBernd->name()<<" filter before GrainFilter";
	} else if (m_hub->addFilter(fBernd, PluginHub::After, QString("Curves"))) {
        qDebug() << "Successfully registered "<<fBernd->name()<<" filter after Curves";
	} else {
    	qDebug() << "Failed to register "<<fBernd->name()<<" filter";
	    return false;
	}


	return true;
}

#define SNONE "None"
bool SiliconBonkPlugin::registerOptions()
{
//    const int HiddenOption = 0 | PluginHub::POPTION_DOESNOTPERSIST;///*PluginHub::POPTION_NO_DISPLAY;|*/ PluginHub::POPTION_DOESNOTPERSIST;
    //const int GUIOption =  0;// PluginHub::POPTION_DONT_RESTART;
                                                                                                                                             
	m_hub->addBoolOption  (SiliconBonkFilter::Enable,   "SiliconBonkOn" , "Enabled"       , tr("Enable Plugin"), "SiliconBonkFilter", false, false, 0);
    m_hub->addBoolOption  (SiliconBonkFilter::ProtectHighlights,"SiliconBonkHighlights", "Highlight Protection", tr("Reduce highlight luminance\nThis increases Highlight details when using high contrast"),"SiliconBonkFilter", DEFAULT_HIGHLIGHT, true, 0);
	
    m_hub->addIntOption   (SiliconBonkFilter::H     ,   "SiliconBonkH"  , "Hue"           , tr("Selected Hue"), "SiliconBonkFilter", DEFAULT_H, 0, 0);  
    m_hub->addIntOption   (SiliconBonkFilter::L     ,   "SiliconBonkL"  , "Luminance"     , tr("Luminance Boost"), "SiliconBonkFilter", DEFAULT_L, 0, 0);
    m_hub->addIntOption   (SiliconBonkFilter::Contrast, "SiliconBonkContrast", "Contrast" , tr("Contrast"), "SiliconBonkFilter", DEFAULT_CONTRAST, 0, 0);    
    m_hub->addIntOption   (SiliconBonkFilter::Mid,      "SiliconBonkMid"  , "Mids"        , tr("Midpoint"), "SiliconBonkFilter", DEFAULT_MID, 0, 0);    
    m_hub->addIntOption   (SiliconBonkFilter::Sat,      "SiliconBonkSat"  , "Saturation"  , tr("Saturation (keep color)"), "SiliconBonkFilter", DEFAULT_SAT, 0, 0); 
    

    m_hub->addIntOption   (SiliconBonkFilter::C0L   ,   "SiliconBonkC0L", "EQ red"        , tr("Luminance EQ red"), "SiliconBonkFilter", DEFAULT_EQUALIZER, 0, 0);
    m_hub->addIntOption   (SiliconBonkFilter::C1L   ,   "SiliconBonkC1L", "EQ yellow"     , tr("Luminance EQ yellow"), "SiliconBonkFilter", DEFAULT_EQUALIZER, 0, 0);
    m_hub->addIntOption   (SiliconBonkFilter::C2L   ,   "SiliconBonkC2L", "EQ green"      , tr("Luminance EQ green"), "SiliconBonkFilter", DEFAULT_EQUALIZER, 0, 0);
    m_hub->addIntOption   (SiliconBonkFilter::C3L   ,   "SiliconBonkC3L", "EQ cyan"       , tr("Luminance EQ cyan"), "SiliconBonkFilter", DEFAULT_EQUALIZER, 0, 0);
    m_hub->addIntOption   (SiliconBonkFilter::C4L   ,   "SiliconBonkC4L", "EQ blue"       , tr("Luminance EQ blue"), "SiliconBonkFilter", DEFAULT_EQUALIZER, 0, 0);
    m_hub->addIntOption   (SiliconBonkFilter::C5L   ,   "SiliconBonkC5L", "EQ magenta"    , tr("Luminance EQ magenta"), "SiliconBonkFilter", DEFAULT_EQUALIZER, 0, 0);
    
   

	return true;
}

bool SiliconBonkPlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));

	return true;
}

PluginDependency *SiliconBonkPlugin::createDependency(const QString &dname)
{
    qDebug()<<PLUGIN_NAME_HR<<" Dependency requested";
	if (dname == "ToolData") {        //For asPluginupdate
        ToolData *toolData = new ToolData(m_hub);
        if (toolData) {
            toolData->owner = this->name();
            toolData->group = this->group();
            toolData->ownerId = m_id;
            toolData->groupId = m_groupId;
            toolData->addEnabledId(m_hub->optionIdForName("Enabled", m_id));
            return toolData;
        }
    }
	return NULL;
}

QList<QString> SiliconBonkPlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> SiliconBonkPlugin::toolWidgets()
{
	return QList<QWidget*>();
}


void SiliconBonkPlugin::toolWidgetCreated(QWidget *uiWidget)
{
    //Q_UNUSED(uiWidget);
    if(uiWidget == NULL) return; //protection
    
    if (QPushButton *rbtn = uiWidget->findChild<QPushButton*>("SiliconBonkReset_btn")) {
			connect(rbtn, SIGNAL(clicked()), this, SLOT(reset()));
		}

}

//========================================================================================================
void SiliconBonkPlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{

    Q_UNUSED(options);
    Q_UNUSED(changed);
    Q_UNUSED(currentLayer);
}

void SiliconBonkPlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
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
	if(
	    changes.contains(SiliconBonkFilter::H, m_groupId)
	  ||changes.contains(SiliconBonkFilter::L, m_groupId)
	  ||changes.contains(SiliconBonkFilter::Contrast, m_groupId)
	  ||changes.contains(SiliconBonkFilter::Mid, m_groupId)
	  ||changes.contains(SiliconBonkFilter::Sat, m_groupId)
	  ||changes.contains(SiliconBonkFilter::ProtectHighlights, m_groupId)
	  ||changes.contains(SiliconBonkFilter::C0L, m_groupId)
	  ||changes.contains(SiliconBonkFilter::C1L, m_groupId)
	  ||changes.contains(SiliconBonkFilter::C2L, m_groupId)
	  ||changes.contains(SiliconBonkFilter::C3L, m_groupId)
	  ||changes.contains(SiliconBonkFilter::C4L, m_groupId)
	  ||changes.contains(SiliconBonkFilter::C5L, m_groupId)
	){
        changes.setBool(SiliconBonkFilter::Enable, m_groupId, true);
	}
	
	
}

//========================================================================================================
void SiliconBonkPlugin::handleHotnessChanged(const PluginImageSettings &options)
{

    Q_UNUSED(options);
}

void SiliconBonkPlugin::reset() {
    if (PluginOptionList *options = m_hub->beginSettingsChange("Reset Silicon Bonk")) {
		options->setInt(SiliconBonkFilter::H, m_groupId, DEFAULT_H);
		options->setInt(SiliconBonkFilter::L, m_groupId, DEFAULT_L);
		options->setInt(SiliconBonkFilter::Contrast, m_groupId, DEFAULT_CONTRAST);
		options->setInt(SiliconBonkFilter::Mid, m_groupId, DEFAULT_MID);
		options->setInt(SiliconBonkFilter::Sat, m_groupId, DEFAULT_SAT);
		options->setBool(SiliconBonkFilter::ProtectHighlights, m_groupId, DEFAULT_HIGHLIGHT);
		options->setInt(SiliconBonkFilter::C0L, m_groupId, DEFAULT_EQUALIZER);
		options->setInt(SiliconBonkFilter::C1L, m_groupId, DEFAULT_EQUALIZER);
		options->setInt(SiliconBonkFilter::C2L, m_groupId, DEFAULT_EQUALIZER);
		options->setInt(SiliconBonkFilter::C3L, m_groupId, DEFAULT_EQUALIZER);
		options->setInt(SiliconBonkFilter::C4L, m_groupId, DEFAULT_EQUALIZER);
		options->setInt(SiliconBonkFilter::C5L, m_groupId, DEFAULT_EQUALIZER);
		m_hub->endSettingChange();
	}
}


