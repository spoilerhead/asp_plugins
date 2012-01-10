#include "GrainPlugin.h"
#include "GrainFilter.h"
#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>
#include <QSettings>

extern "C" BIBBLE_API BaseB5Plugin *b5plugin() { return new GrainPlugin; }


bool GrainPlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	//m_seed = NULL;
	qDebug()<<"Grain Filter init...";
	
	// Store our group ID and group name (andrewj)
    QSettings oSettings("Bibble and AfterShot Plugins", "PluginGroups");
    oSettings.setValue(group(), groupId);
	return true;
}

bool GrainPlugin::registerFilters()
{
    qDebug()<<"Grain Filter register...";
	GrainFilter *mirror = new GrainFilter(m_hub, m_groupId);
	if (m_hub->addFilter(mirror, PluginHub::After/*Before*/, QString("Curves"))) {
//	if (m_hub->addFilter(mirror, PluginHub::After, QString("RgbToLab"))) {
//	if (m_hub->addFilter(mirror, PluginHub::After, QString("Sharpen"))) {
	    qDebug()<<"SPH Grain Filter Registered"<<endl;
		return true;
	}
	qDebug() <<"Grain Filter init... failed";
	qDebug() << "Failed to load the Grain filter";
	return false;
}

//! Setting doesn't get saved with the version
//#define OPTION_DOESNOTPERSIST	8
#define POPTION_DOESNOTPERSIST 8
#define POPTION_NO_DISPLAY 16384 
//FIXME, this is stolen from 5.0 plugin, and seems to work, but its not documented

bool GrainPlugin::registerOptions()
                            //option id             option name (.ui)      selective copy       Hint                filtername    default   zero  flags
{
    
	m_hub->addBoolOption  (GrainFilter::Enable,    "bSphGrainon"        , "Grain Enabled"   , tr("Enables Grain","Mouse over hint"), "GrainFilter", false, false, 0);
	m_hub->addIntOption(GrainFilter::Radius,       "bSphGrainRadius"    , "Grain Radius"    , tr("Increases the Radius of the Blur filter","Mouse over hint"), "GrainFilter", 10, 0, 0);
	m_hub->addIntOption   (GrainFilter::Seed,      "bSphGrainSeed"      , "Grain Seed"      , tr("Random Seed","Mouse over hint"), "GrainFilter", 0, 0,0);
	m_hub->addIntOption   (GrainFilter::HiddenSeed,"bSphGrainHiddenSeed", "Grain hiddenSeed"    ,  "Random Seed",                  "GrainFilter", 0, 0,0|POPTION_DOESNOTPERSIST|POPTION_NO_DISPLAY);
    m_hub->addIntOption   (GrainFilter::Amount,    "bSphGrainAmount"    , "Grain Amount"    , tr("Amount","Mouse over hint"), "GrainFilter", 25, 0, 0);
    m_hub->addIntOption   (GrainFilter::Shadow,    "bSphGrainShadow"    , "Grain Shadow"    , tr("Shadow Strength","Mouse over hint"), "GrainFilter", 30, 0, 0);
    m_hub->addIntOption   (GrainFilter::Mid,       "bSphGrainMid"       , "Grain Mid"       , tr("Midtone Strength","Mouse over hint"), "GrainFilter", 60, 0, 0);
    m_hub->addIntOption   (GrainFilter::Light,     "bSphGrainLight"     , "Grain Light"     , tr("Lights Strength","Mouse over hint"), "GrainFilter", 20, 0, 0);
    m_hub->addBoolOption  (GrainFilter::Random,    "bSphGrainRandom"    , "Grain Random"    , tr("Randomize Seed","Mouse over hint"), "GrainFilter", false, 0, 0);
    m_hub->addBoolOption  (GrainFilter::ColorGrain,"bSphGrainColorGrain", "Grain Color"     , tr("Colored Grain","Mouse over hint"), "GrainFilter", false, false, 0);
    
    

	return true;
}

bool GrainPlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *GrainPlugin::createDependency(const QString &name)
{
	Q_UNUSED(name);
	return NULL;
}

QList<QString> GrainPlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> GrainPlugin::toolWidgets()
{
	return QList<QWidget*>();
}

void GrainPlugin::toolWidgetCreated(QWidget *uiWidget)
{
    Q_UNUSED(uiWidget);
/*	// There is an extra combobox in the standard tool (made from the bpxml).  It is not named
	//   correctly for the auto-connection so let's find that bad boy and hook him up to some
	//   internal logic

	// I added a dynamic property "toolType" so I can figure out which tool is which.
	QVariant type = uiWidget->property("toolType");
	//if (type != QVariant() && type.toInt() == 0) 
	{		// Looking for tool 0
		// Find the combo box, it is named "flipper"
		qDebug()<<"looking for box";
		if (QSpinBox *seed = uiWidget->findChild<QSpinBox*>("bSphGrainSeed_edit")) {
			m_seed = seed;		// cache this off, will be using it a lot.
		}
	}*/
}

//========================================================================================================
void GrainPlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{
    Q_UNUSED(changed);
    if(options.options(currentLayer) == NULL) return;
    //qDebug()<<"GRAIN: handleSettingsChange";
  /*  
    if (
	    options.options(currentLayer)->contains(GrainFilter::Random, m_groupId)
	){
	qDebug()<<"random";
	    bool okay;
	    bool randomSeed = options.options(currentLayer)->getBool(GrainFilter::Random, m_groupId,okay);
	    //qDebug()<<randomSeed<<" "<<okay;
	    if(okay && randomSeed) {
	        int newSeed = (rand()%256);
            if(m_seed) {
                m_seed->blockSignals(true);    // Block signals so we don't start a change
                m_seed->setValue(newSeed);
                m_seed->blockSignals(false);
            		qDebug()<<"box SET";
            }


            //options.options(currentLayer) ->setInt(GrainFilter::Seed, m_groupId, newSeed);


        }
    }*/
    
    /*bool okay;
    bool randomized = options.options(currentLayer)->getBool(GrainFilter::Random, m_groupId, okay);
    if(randomized) options.options(currentLayer)->setInt(GrainFilter::HiddenSeed, m_groupId, (rand()%256));*/
}

void GrainPlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
{
    Q_UNUSED(optionName);
    Q_UNUSED(groupId);
    Q_UNUSED(current);
    //qDebug()<<"GRAIN: handleControlsChange";
    //   v-- You need to add this if you're going to use the options --v
    //   some changes do not have options - check for NULL
    if (options.options(layer) == NULL) return;
    
    // Grain settings changes   
	if(
	    changes.contains(GrainFilter::Radius, m_groupId)
	    || changes.contains(GrainFilter::Seed, m_groupId)
	    || changes.contains(GrainFilter::Amount, m_groupId)
	    || changes.contains(GrainFilter::Shadow, m_groupId)
	    || changes.contains(GrainFilter::Mid, m_groupId)
	    || changes.contains(GrainFilter::Light, m_groupId)
	    || changes.contains(GrainFilter::Random, m_groupId)
	    || changes.contains(GrainFilter::ColorGrain, m_groupId)
	){
	    bool okay;
		bool enabled = options.options(layer)->getBool(GrainFilter::Enable, m_groupId, okay);
		if (enabled == false) 
            changes.setBool(GrainFilter::Enable, m_groupId, true);
	}
	
	/*if(
	    changes.contains(GrainFilter::Random, m_groupId)
	){
	    bool okay;
		bool randomized = options.options(layer)->getBool(GrainFilter::Random, m_groupId, okay);
		if (randomized == false) {
            changes.setInt(GrainFilter::HiddenSeed, m_groupId, (rand()%256));

        }
	}*/

	

}

//========================================================================================================
void GrainPlugin::handleHotnessChanged(const PluginImageSettings &options)
{
    Q_UNUSED(options);
    //qDebug()<<"GRAIN: handleHotnessChange";
}

