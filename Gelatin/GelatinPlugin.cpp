#include "GelatinPlugin.h"
#include "FilterGelatin.h"


#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>


#include <QWidget>


#define PLUGIN_NAME_HR "Gelatin"



extern "C" BIBBLE_API B5Plugin *b5plugin() { return new GelatinPlugin; }


bool GelatinPlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
    
    if(hub == NULL) return false;
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	return true;
}

bool GelatinPlugin::registerFilters()
{
    GelatinFilter *fGelatin = new GelatinFilter(m_hub, m_groupId);
	if (m_hub->addFilter(fGelatin, PluginHub::Before, QString("LabToRgb"))) {
        qDebug() << "Successfully registered "<<fGelatin->name()<<" filter";
	} else {
    	qDebug() << "Failed to register "<<fGelatin->name()<<" filter";
	    return false;
	}


	return true;
}

#define SNONE "None"
bool GelatinPlugin::registerOptions()
{
//    const int HiddenOption = 0 | PluginHub::POPTION_DOESNOTPERSIST;///*PluginHub::POPTION_NO_DISPLAY;|*/ PluginHub::POPTION_DOESNOTPERSIST;
    const int GUIOption =  0;// PluginHub::POPTION_DONT_RESTART;
                                                                                                                                             //default, zero,0
	m_hub->addBoolOption  (GelatinFilter::Enable, "GelatinOn"                       , "Enabled"         , tr("Enable"), "GelatinFilter", false, false, 0);
    m_hub->addBoolOption  (GelatinFilter::StretchContrast, "GelatinStretchContrast" , "Stretch Contrast", tr("Stretch Contrast (Autocontrast)"), "GelatinFilter", false, false, 0);

    m_hub->addStringOption   (GelatinFilter::ColorFilter,"GelatinColorFilter"          , "Color Filter"    , tr("Color filters"), "GelatinFilter", SNONE, SNONE, GUIOption);  
//    m_hub->addIntOption   (GelatinFilter::ColorFilter,"GelatinColorFilterHidden"              , "Color Filter Int"      , "", "GelatinFilter", 0, 0, HiddenOption);   
    
   //ColorFilterGUI=16,DeveloperGUI=17,PaperGUI=18 
    m_hub->addStringOption   (GelatinFilter::FilmStock,"GelatinFilmStock"              , "Film Stock"      , tr("Film stock (spectral response)"), "GelatinFilter", SNONE, SNONE,  GUIOption);
//    m_hub->addIntOption   (GelatinFilter::FilmStock,"GelatinFilmStockHidden"              , "Film Stock Int"      , "", "GelatinFilter", 0, 0, HiddenOption);
    
    m_hub->addStringOption(GelatinFilter::Developer,"GelatinDeveloper"              , "Developer"       , tr("Film Developer (contrast)"), "GelatinFilter", SNONE, SNONE,  GUIOption);
//    m_hub->addIntOption   (GelatinFilter::Developer,"GelatinDeveloperHidden"              , "Developer Int"       , "", "GelatinFilter", 0, 0, HiddenOption);
    
    
    m_hub->addStringOption(GelatinFilter::Paper,"GelatinPaper"                      , "Paper"           , tr("Paper (contrast)"), "GelatinFilter", SNONE, SNONE,  GUIOption);
//    m_hub->addIntOption   (GelatinFilter::Paper,"GelatinPaperHidden"                   , "Paper Int"           , "","", 0, 0, HiddenOption);  
    
       
        	
    m_hub->addDoubleOption(GelatinFilter::EXPFilm,    "GelatinEXPFilm"              , "exposure film"   , tr("Film Exposure"), "GelatinFilter", 0, 0, 0);
    m_hub->addDoubleOption(GelatinFilter::EXPPaper,   "GelatinEXPPaper"             , "exposure paper"  , tr("Paper Exposure"), "GelatinFilter", 0, 0, 0);
	
    m_hub->addBoolOption  (GelatinFilter::Color, "GelatinColor"                     , "Color"           , tr("Color mode"), "GelatinFilter", false, false, 0);
    m_hub->addBoolOption  (GelatinFilter::BaseTint, "GelatinTint"                   , "Tint"            , tr("Apply Base Tint"), "GelatinFilter", false, false, 0);



//    m_hub->addIntOption   (GelatinFilter::IntensityBoost,"GelatinIntensityBoost"    , "dbg:intboost"    , tr("intensity boost"), "GelatinFilter", 100, 0, 0);  


//    m_hub->addIntOption   (GelatinFilter::TintBoost,"GelatinTintBoost"              , "dbg:tintboost"   , tr("Tint boost"), "GelatinFilter", 100, 0, 0);  
//    m_hub->addBoolOption  (GelatinFilter::ColorShift, "GelatinColorShift"       , "Color Shift"     , tr("Color Shift"), "GelatinFilter", false, false, 0);

    m_hub->addIntOption   (GelatinFilter::Colorbleed,"GelatinColorBleed"            , "Color Bleed"   , tr("Amount of original color mixed back"), "GelatinFilter", 0, 0, 0);  





  
	return true;
}

bool GelatinPlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *GelatinPlugin::createDependency(const QString &name)
{
	Q_UNUSED(name);
	return NULL;
}

QList<QString> GelatinPlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> GelatinPlugin::toolWidgets()
{
	return QList<QWidget*>();
}
#include <QAbstractItemView>
void GelatinPlugin::toolWidgetCreated(QWidget *uiWidget)
{
    //Q_UNUSED(uiWidget);
    if(uiWidget == NULL) return; //protection

    QVariant type = uiWidget->property("BIBBLE_PLUGIN_NAME");
	if (type != QVariant() && type.toString() == "Gelatin") {		// Looking for tool 0
		// Find the combo box, it is named "flipper"
		if (QComboBox *combo = uiWidget->findChild<QComboBox*>("GelatinPaper_combo")) {
            fillPaperCombo(combo);
            combo->view()->setFixedWidth(250);
             combo->view()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		}
		if (QComboBox *combo = uiWidget->findChild<QComboBox*>("GelatinDeveloper_combo")) {
            fillDeveloperCombo(combo);
            combo->view()->setFixedWidth(250);
             combo->view()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		}
		if (QComboBox *combo = uiWidget->findChild<QComboBox*>("GelatinFilmStock_combo")) {
            fillFilmStockCombo(combo);
             combo->view()->setFixedWidth(250);
             combo->view()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		}
		if (QComboBox *combo = uiWidget->findChild<QComboBox*>("GelatinColorFilter_combo")) {
            fillColorFiltersCombo(combo);
            combo->view()->setFixedWidth(250);
             combo->view()->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
		}

	}
}

//========================================================================================================
void GelatinPlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{

    Q_UNUSED(options);
    Q_UNUSED(changed);
    Q_UNUSED(currentLayer);

    
 /*   if(currentLayer != 0) {
        qDebug()<<"FIXME, layers currently broken, ask bibble";
        return;
    }
    if (options.options(currentLayer) == NULL) return;

    if (changed.options(currentLayer) == NULL) return; //<--- this line crashes as soon as currentLayer != 0

    bool okay;
    
#define HAS_CHANGED(x) changed.options(currentLayer)->contains(x, m_groupId)
#define GET_STRING(x) changed.options(currentLayer)->getString(x,m_groupId, okay);
    
   
    
    if(HAS_CHANGED(GelatinFilter::FilmStockGUI)) {
	    QString filmName = GET_STRING(GelatinFilter::FilmStockGUI);
	    int filmID = getFilmID(filmName);
	    
        PluginOptionList *cur = m_hub->beginSettingsChange("");
        if(cur != NULL) cur->setInt(GelatinFilter::FilmStock,m_groupId, filmID);
        m_hub->endSettingChange();
	}
	if(HAS_CHANGED(GelatinFilter::ColorFilterGUI)) {
	    QString filterName = GET_STRING(GelatinFilter::ColorFilterGUI);
	    int filterID = getColorFilterID(filterName);

        PluginOptionList *cur = m_hub->beginSettingsChange("");
        if(cur != NULL) cur->setInt(GelatinFilter::ColorFilter,m_groupId, filterID);
        m_hub->endSettingChange();
	}
	if(HAS_CHANGED(GelatinFilter::DeveloperGUI)) {
	    QString DeveloperName = GET_STRING(GelatinFilter::DeveloperGUI);
	    int DeveloperID = getDeveloperID(DeveloperName);
	    
        PluginOptionList *cur = m_hub->beginSettingsChange("");
        if(cur != NULL) cur->setInt(GelatinFilter::Developer,m_groupId, DeveloperID);
        m_hub->endSettingChange();
	}
	if(HAS_CHANGED(GelatinFilter::PaperGUI)) {
	    QString PaperName = GET_STRING(GelatinFilter::PaperGUI);
	    int PaperID = getPaperID(PaperName);

        PluginOptionList *cur = m_hub->beginSettingsChange("");
        if(cur != NULL) cur->setInt(GelatinFilter::Paper,m_groupId, PaperID);
        m_hub->endSettingChange();
	}
	*/
}

void GelatinPlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
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
	    changes.contains(GelatinFilter::Color, m_groupId)
	  ||changes.contains(GelatinFilter::StretchContrast, m_groupId)  
	  ||changes.contains(GelatinFilter::ColorFilter, m_groupId)
	  ||changes.contains(GelatinFilter::FilmStock, m_groupId)
	  ||changes.contains(GelatinFilter::Developer, m_groupId)
	  ||changes.contains(GelatinFilter::Paper, m_groupId)	  
	  ||changes.contains(GelatinFilter::EXPFilm, m_groupId)
	  ||changes.contains(GelatinFilter::EXPPaper, m_groupId)
	  ||changes.contains(GelatinFilter::BaseTint, m_groupId)
	  ||changes.contains(GelatinFilter::Colorbleed, m_groupId)
	   //|| changes.contains(USMFilter::AmountUSM, m_groupId)
	){
        changes.setBool(GelatinFilter::Enable, m_groupId, true);
	}
	
	
}

//========================================================================================================
void GelatinPlugin::handleHotnessChanged(const PluginImageSettings &options)
{

    Q_UNUSED(options);
}
