#include "FatToniPlugin.h"
#include "FilterFatToni.h"


#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>


#include <QWidget>
#include <QPushButton>


#define PLUGIN_NAME_HR "FatToni"

#define DEFAULT_HH 0
#define DEFAULT_SH 0
#define DEFAULT_HS 0
#define DEFAULT_SS 0
#define DEFAULT_MID 0
#define DEFAULT_MIX 0
#define DEFAULT_CONT 85
#define DEFAULT_BASEH 0
#define DEFAULT_BASES 0
#define DEFAULT_BASEL 100

extern "C" BIBBLE_API B5Plugin *b5plugin() { return new FatToniPlugin; }


bool FatToniPlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
    
    if(hub == NULL) return false;
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	return true;
}

bool FatToniPlugin::registerFilters()
{
    FatToniFilter *fToni = new FatToniFilter(m_hub, m_groupId);
    
    
    if (m_hub->addFilter(fToni, PluginHub::After, QString("GrainFilter"))) {
        qDebug() << "Successfully registered "<<fToni->name()<<" filter after GrainFilter";
    } else if (m_hub->addFilter(fToni, PluginHub::After, QString("SiliconBonkFilter"))) {
        qDebug() << "Successfully registered "<<fToni->name()<<" filter after SiliconBonk";
	} else if (m_hub->addFilter(fToni, PluginHub::After, QString("Curves"))) {
        qDebug() << "Successfully registered "<<fToni->name()<<" filter after Curves";
	} else {
    	qDebug() << "Failed to register "<<fToni->name()<<" filter";
	    return false;
	}


	return true;
}

#define SNONE "None"
bool FatToniPlugin::registerOptions()
{
//    const int HiddenOption = 0 | PluginHub::POPTION_DOESNOTPERSIST;///*PluginHub::POPTION_NO_DISPLAY;|*/ PluginHub::POPTION_DOESNOTPERSIST;
    //const int GUIOption =  0;// PluginHub::POPTION_DONT_RESTART;
                                                                                                                                             
	m_hub->addBoolOption  (FatToniFilter::Enable,   "FatToniOn" , "Enabled"       , tr("Enable Plugin"), "FatToniFilter", false, false, 0);
    
    m_hub->addIntOption   (FatToniFilter::HH     ,   "FatToniHH" , "Highlight Hue"           , tr("Selected Hue"), "FatToniFilter", DEFAULT_HH, 0, 0);  
    m_hub->addIntOption   (FatToniFilter::SH     ,   "FatToniSH" , "Highlight Saturation"     , tr("Saturation"), "FatToniFilter", DEFAULT_SH, 0, 0);
    
    m_hub->addIntOption   (FatToniFilter::HS     ,   "FatToniHS" , "Shadow Hue"           , tr("Selected Hue"), "FatToniFilter", DEFAULT_HS, 0, 0);  
    m_hub->addIntOption   (FatToniFilter::SS     ,   "FatToniSS" , "Shadow Saturation"     , tr("Saturation"), "FatToniFilter", DEFAULT_SS, 0, 0);    
    
    
    m_hub->addIntOption   (FatToniFilter::Mid    , "FatToniMid"  , "Midpoint"     , tr("Midpoint"), "FatToniFilter", DEFAULT_MID, 0, 0);  
    m_hub->addIntOption   (FatToniFilter::Mix    , "FatToniMix"  , "Separation"     , tr("Color Separation"), "FatToniFilter", DEFAULT_MIX, 0, 0);  
    m_hub->addIntOption   (FatToniFilter::Cont   , "FatToniCont" , "Contrast"     , tr("Contrast"), "FatToniFilter", DEFAULT_CONT, 0, 0);  
    
    m_hub->addIntOption   (FatToniFilter::BaseH  , "FatToniBaseH", "Base Hue"       , tr("Base Hue"), "FatToniFilter", DEFAULT_BASEH, 0, 0);
    m_hub->addIntOption   (FatToniFilter::BaseS  , "FatToniBaseS", "Base Saturation", tr("Base Saturation"), "FatToniFilter", DEFAULT_BASES, 0, 0);
    m_hub->addIntOption   (FatToniFilter::BaseL  , "FatToniBaseL", "Base Luminance" , tr("Base Luminance"), "FatToniFilter", DEFAULT_BASEL, 0, 0);
    
    
    
    
    
    /*m_hub->addIntOption   (FatToniFilter::Contrast, "FatToniContrast", "Contrast" , tr("Contrast"), "FatToniFilter", 0, 0, 0);    
    m_hub->addIntOption   (FatToniFilter::Mid,      "FatToniMid"  , "Mid"         , tr("Midpoint"), "FatToniFilter", 0, 0, 0);    
*/
    

	return true;
}

bool FatToniPlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *FatToniPlugin::createDependency(const QString &name)
{
	Q_UNUSED(name);
	return NULL;
}

QList<QString> FatToniPlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> FatToniPlugin::toolWidgets()
{
	return QList<QWidget*>();
}

void FatToniPlugin::toolWidgetCreated(QWidget *uiWidget)
{
    //Q_UNUSED(uiWidget);
    if(uiWidget == NULL) return; //protection
    
    
    if (QPushButton *rbtn = uiWidget->findChild<QPushButton*>("FatToniReset_btn")) {
			connect(rbtn, SIGNAL(clicked()), this, SLOT(reset()));
		}

}

//========================================================================================================
void FatToniPlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{

    Q_UNUSED(options);
    Q_UNUSED(changed);
    Q_UNUSED(currentLayer);
}

void FatToniPlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
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
	    changes.contains(FatToniFilter::HH, m_groupId)
	  ||changes.contains(FatToniFilter::SH, m_groupId)
	  ||changes.contains(FatToniFilter::HS, m_groupId)
	  ||changes.contains(FatToniFilter::SS, m_groupId)
	  ||changes.contains(FatToniFilter::Cont, m_groupId)
	  ||changes.contains(FatToniFilter::Mid, m_groupId)
	  ||changes.contains(FatToniFilter::Mix, m_groupId)
	  ||changes.contains(FatToniFilter::BaseH, m_groupId)
	  ||changes.contains(FatToniFilter::BaseS, m_groupId)
	  ||changes.contains(FatToniFilter::BaseL, m_groupId)
	){
        changes.setBool(FatToniFilter::Enable, m_groupId, true);
	}
	
	
}

//========================================================================================================
void FatToniPlugin::handleHotnessChanged(const PluginImageSettings &options)
{

    Q_UNUSED(options);
}

void FatToniPlugin::reset() {
    if (PluginOptionList *options = m_hub->beginSettingsChange("Reset Fat Toni")) {
		options->setInt(FatToniFilter::HH, m_groupId, DEFAULT_HH);
		options->setInt(FatToniFilter::SH, m_groupId, DEFAULT_SH);
		options->setInt(FatToniFilter::HS, m_groupId, DEFAULT_HS);
		options->setInt(FatToniFilter::SS, m_groupId, DEFAULT_SS);
		options->setInt(FatToniFilter::Mid, m_groupId, DEFAULT_MID);
		options->setInt(FatToniFilter::Mix, m_groupId, DEFAULT_MIX);
		options->setInt(FatToniFilter::Cont, m_groupId, DEFAULT_CONT);
		options->setInt(FatToniFilter::BaseH, m_groupId, DEFAULT_BASEH);
		options->setInt(FatToniFilter::BaseS, m_groupId, DEFAULT_BASES);
		options->setInt(FatToniFilter::BaseL, m_groupId, DEFAULT_BASEL);
		m_hub->endSettingChange();
	}
}
