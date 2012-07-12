#include "AlicePlugin.h"

#include "FilterPyramid.h"
#include "PluginHub.h"
#include "PluginDependency.h"

#include "ToolData.h"

#include <QDebug>
#include <iostream>
#include <QSettings>

#define PLUGIN_NAME_HR "aLiCE"

extern "C" BIBBLE_API BaseB5Plugin *b5plugin() { return new AlicePlugin; }


bool AlicePlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
    qDebug() << "Initialising "<<PLUGIN_NAME_HR<<" plugin";
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	
	// Store our group ID and group name (andrewj)
    QSettings oSettings("Bibble and AfterShot Plugins", "PluginGroups");
    oSettings.setValue(group(), groupId);
	return true;
}

bool AlicePlugin::registerFilters()
{
    PyramidFilter *pyramid = new PyramidFilter(m_hub, m_groupId);
		if (m_hub->addFilter(pyramid, PluginHub::Before, QString("Curves"))) {
        qDebug() << "Successfully registered "<<pyramid->name()<<" filter";
	} else {
    	qDebug() << "Failed to register "<<pyramid->name()<<" filter";
	    return false;
	}
	
	qDebug() << "All "<<PLUGIN_NAME_HR<<" Filters enabled";
	return true;
}

bool AlicePlugin::registerOptions()
{
	m_hub->addBoolOption  (PyramidFilter::EnablePyramid, "bPyramidOn", "Pyramid Enabled"  , tr("Enable Pyramid"), "aLiCE_Pyramid_Filter", false, false, 0);
	m_hub->addIntOption   (PyramidFilter::P1,            "bPyramid1" , "Pyramid1"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P2,            "bPyramid2" , "Pyramid2"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P3,            "bPyramid3" , "Pyramid3"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P4,            "bPyramid4" , "Pyramid4"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P5,            "bPyramid5" , "Pyramid5"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P6,            "bPyramid6" , "Pyramid6"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P7,            "bPyramid7" , "Pyramid7"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	m_hub->addIntOption   (PyramidFilter::P8,            "bPyramid8" , "Pyramid8"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
//	m_hub->addIntOption   (PyramidFilter::P9,            "bPyramid9" , "Pyramid9"         , tr(""), "aLiCE_Pyramid_Filter", 100, 0, 0);

    m_hub->addIntOption   (PyramidFilter::Shadow,        "bPyramidS" , "PyramidS"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
    m_hub->addIntOption   (PyramidFilter::Mid,           "bPyramidM" , "PyramidM"         , tr(""), "aLiCE_Pyramid_Filter", 100, 100, 0);
    m_hub->addIntOption   (PyramidFilter::Light,         "bPyramidL" , "PyramidL"         , tr(""), "aLiCE_Pyramid_Filter", 0, 0, 0);
	

    
	return true;
}

bool AlicePlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *AlicePlugin::createDependency(const QString &dname)
{
    qDebug()<<PLUGIN_NAME_HR<<" Dependency requested";
    if (dname == "ToolData") {        //For asPluginupdate
        ToolData *toolData = new ToolData();
        if (toolData) {
            toolData->owner = this->name();
            toolData->group = this->group();
            toolData->ownerId = m_id;
            toolData->groupId = m_groupId;
            toolData->addEnabledId(m_hub->optionIdForName("bPyramidOn", m_id),"Pyr","Pyramid");
            return toolData;
        }
    }
   return NULL;
}

QList<QString> AlicePlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> AlicePlugin::toolWidgets()
{
	return QList<QWidget*>();
}

void AlicePlugin::toolWidgetCreated(QWidget *uiWidget)
{
    Q_UNUSED(uiWidget);
}

//========================================================================================================
void AlicePlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{
    Q_UNUSED(options);
    Q_UNUSED(changed);
    Q_UNUSED(currentLayer);
    
    if(options.options(currentLayer) == NULL) return;
}

void AlicePlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
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
	    changes.contains(PyramidFilter::P1, m_groupId)
	    || changes.contains(PyramidFilter::P2, m_groupId)
	    || changes.contains(PyramidFilter::P3, m_groupId)
	    || changes.contains(PyramidFilter::P4, m_groupId)
	    || changes.contains(PyramidFilter::P5, m_groupId)
	    || changes.contains(PyramidFilter::P6, m_groupId)
	    || changes.contains(PyramidFilter::P7, m_groupId)
	    || changes.contains(PyramidFilter::P8, m_groupId)
	    || changes.contains(PyramidFilter::Shadow, m_groupId)
	    || changes.contains(PyramidFilter::Mid, m_groupId)
	    || changes.contains(PyramidFilter::Light, m_groupId)
	    //|| changes.contains(PyramidFilter::P9, m_groupId)
	    
	){
        changes.setBool(PyramidFilter::EnablePyramid, m_groupId, true);
	}

}

//========================================================================================================
void AlicePlugin::handleHotnessChanged(const PluginImageSettings &options)
{
    Q_UNUSED(options);
}

