#include "SharpenPlugin.h"

#include "FilterSharpen.h"
#include "FilterUSM.h"
#include "FilterLLorens.h"
#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>
#include <QSettings>

#define PLUGIN_NAME_HR "Wavelet Sharpen 3"

extern "C" BIBBLE_API BaseB5Plugin *b5plugin() { return new SharpenPlugin; }


bool SharpenPlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
{
    Q_UNUSED(bundlePath);
    qDebug() << "Initialising "<<PLUGIN_NAME_HR<<" filter";
	m_hub = hub;
	m_id = id;
	m_groupId = groupId;
	
	// Store our group ID and group name (andrewj)
    QSettings oSettings("Bibble and AfterShot Plugins", "PluginGroups");
    oSettings.setValue(group(), groupId);
	return true;
}

bool SharpenPlugin::registerFilters()
{
    LLorensFilter *llorens = new LLorensFilter(m_hub, m_groupId);
	//if (m_hub->addFilter(llorens, PluginHub::After, QString("RgbToLab"))) {
		if (m_hub->addFilter(llorens, PluginHub::Before, QString("Sharpen"))) {
        qDebug() << "Successfully registered "<<llorens->name()<<" filter";
	} else {
    	qDebug() << "Failed to register "<<llorens->name()<<" filter";
	    return false;
	}


	USMFilter *usm = new USMFilter(m_hub, m_groupId);
	if (m_hub->addFilter(usm, PluginHub::Before, QString("RgbToLab"))) {
        qDebug() << "Successfully registered "<<usm->name()<<" filter";
	} else {
    	qDebug() << "Failed to register "<<usm->name()<<" filter";
	    return false;
	}
	
	
	WaveletSharpenFilter *sharpener = new WaveletSharpenFilter(m_hub, m_groupId);
	if (m_hub->addFilter(sharpener, PluginHub::After, usm->name())) {
        qDebug() << "Successfully registered "<<sharpener->name()<<" filter";
	} else {
    	qDebug() << "Failed to register "<<sharpener->name()<<" filter";
	    return false;
	}
	
	qDebug() << "All "<<PLUGIN_NAME_HR<<" Filters enabled";
	return true;
}

bool SharpenPlugin::registerOptions()
{

	m_hub->addBoolOption  (USMFilter::EnableUSM, "bSphWaveletUsmon"            , "Usm Enabled"  , tr("Enables Usm"), "WaveletSharpen2_USM_Filter", false, false, 0);
	m_hub->addIntOption   (USMFilter::RadiusUSM, "bSphWaveletUsmRadius"        , "Usm Radius"   , tr("Increases the Radius of the Blur filter"), "WaveletSharpen2_USM_Filter", 50, 0, 0);
    m_hub->addIntOption   (USMFilter::AmountUSM, "bSphWaveletUsmAmount"        , "Usm Amount"   , tr("Amount"), "WaveletSharpen2_USM_Filter", 100, 0, 0);
	m_hub->addIntOption   (USMFilter::ThresholdUSM, "bSphWaveletUsmThreshold"  , "Usm Threshold", tr("Define Minimum Edge for sharpening"), "WaveletSharpen2_USM_Filter", 0, 0, 0);
    m_hub->addBoolOption  (USMFilter::USMasClarity, "bSphWaveletUsmClarity"    , "Clarity Mode" , tr("Makes usm work as clarity control"), "WaveletSharpen2_USM_Filter", false, false, 0);
    
    
	m_hub->addBoolOption  (LLorensFilter::EnableLL, "bSphWaveletLLenable"            , "Gradient Enabled"  , tr("Enables Gradient Sharpening"), "WaveletSharpen2_LLorens_Filter", false, false, 0);
    m_hub->addIntOption   (LLorensFilter::LLStrength, "bSphWaveletLLStrength"        , "Gradient Strength" , tr("Gradient Sharpening Strength"), "WaveletSharpen2_LLorens_Filter", 30, 0, 0);
    m_hub->addIntOption   (LLorensFilter::LLIter, "bSphWaveletLLiter"                , "Gradient Width"    , tr("With of the Gradient (slow)"), "WaveletSharpen2_LLorens_Filter", 3, 1, 0);
    m_hub->addIntOption   (LLorensFilter::MCStrength, "bSphWaveletMCStrength"        , "Microcontrast"      , tr("Amount of Microcontrast to be added"), "WaveletSharpen2_LLorens_Filter", 0, 0, 0);
    
    

	m_hub->addBoolOption  (WaveletSharpenFilter::Enable,   "bSphWaveleton"              , "Sharpen 1 Enable",  tr("Enable Filter"), "WaveletSharpen2_Wavelet_Filter", false, false, 0);
	m_hub->addIntOption   (WaveletSharpenFilter::Amount,   "bSphWaveletAmount"          , "Sharpen 1 Amount",  tr("Sharpening Strength"), "WaveletSharpen2_Wavelet_Filter", 50, 0, 0);
	m_hub->addDoubleOption(WaveletSharpenFilter::Radius,   "bSphWaveletRadius"          , "Sharpen 1 Radius",  tr("Sharpening Radius"), "WaveletSharpen2_Wavelet_Filter", 0.5, 0, 0);
//	m_hub->addIntOption   (WaveletSharpenFilter::Blend,    "bSphWaveletBlend"           , "Sharpen 1 Blend",   "Blend", "WaveletSharpen2_Wavelet_Filter", 100, 0, 0);
	m_hub->addBoolOption  (WaveletSharpenFilter::DePepper, "bSphWaveletSharpenDePepper" , "Sharpen 1 DePepper",tr("Salt and Pepper noise removal (slow)"), "WaveletSharpen2_Wavelet_Filter", false, false, 0);
	m_hub->addIntOption  (WaveletSharpenFilter::Edges,    "bSphWaveletEdge"     , "Sharpen 1 Edge",    tr("Limit Sharpening to Edges"), "WaveletSharpen2_Wavelet_Filter", 100, 0, 0);	
	m_hub->addBoolOption  (WaveletSharpenFilter::Clarity,   "bSphWaveletSharpenClarity" , "Sharpen 1 Clarity",  tr("Apply with clarity mask"), "WaveletSharpen2_Wavelet_Filter", false, false, 0);	
	
	
	
	
	m_hub->addBoolOption  (WaveletSharpenFilter::Enable2,   "bSphWaveleton2"              , "Sharpen 2 Enable",  tr("Enable Filter"), "WaveletSharpen2_Wavelet_Filter", false, false, 0);
	m_hub->addIntOption   (WaveletSharpenFilter::Amount2,   "bSphWaveletAmount2"          , "Sharpen 2 Amount",  tr("Sharpening Strength"), "WaveletSharpen2_Wavelet_Filter", 50, 0, 0);
	m_hub->addDoubleOption(WaveletSharpenFilter::Radius2,   "bSphWaveletRadius2"          , "Sharpen 2 Radius",  tr("Sharpening Radius"), "WaveletSharpen2_Wavelet_Filter", 2.0, 0, 0);
//	m_hub->addIntOption   (WaveletSharpenFilter::Blend2,    "bSphWaveletBlend2"           , "Sharpen 2 Blend",   "Blend", "WaveletSharpen2_Wavelet_Filter", 100, 0, 0);
	m_hub->addBoolOption  (WaveletSharpenFilter::DePepper2, "bSphWaveletSharpenDePepper2" , "Sharpen 2 DePepper", tr("Salt and Pepper noise removal (slow)"), "WaveletSharpen2_Wavelet_Filter", false, false, 0);
	m_hub->addIntOption  (WaveletSharpenFilter::Edges2,    "bSphWaveletEdge2"     , "Sharpen 2 Edge",    tr("Limit Sharpening to Edges"), "WaveletSharpen2_Wavelet_Filter", 100, 0, 0);		
	m_hub->addBoolOption  (WaveletSharpenFilter::Clarity2,   "bSphWaveletSharpenClarity2" , "Sharpen 2 Clarity",  tr("Apply with clarity mask"), "WaveletSharpen2_Wavelet_Filter", false, false, 0);	
    
    /*m_hub->addIntOption   (SharpenFilter::EAWAlpha, "bSphWaveletEAWAlpha"        , "EAWAlpha", "Alpha", "WaveletSharpen2_Wavelet_Filter", 0, 0, 0);
    m_hub->addIntOption   (SharpenFilter::EAWMaxband, "bSphWaveletEAWMaxband"    , "EAWMaxband", "Maxband", "WaveletSharpen2_Wavelet_Filter", 10, 0, 0);
    m_hub->addIntOption   (SharpenFilter::EAWInband, "bSphWaveletEAWInband"     , "EAWInband", "Inband", "WaveletSharpen2_Wavelet_Filter", 5, 0, 0);
    m_hub->addIntOption   (SharpenFilter::EAWMode, "bSphWaveletEAWMode"          , "EAWMode", "Mode", "WaveletSharpen2_Wavelet_Filter", 0, 0, 0);    */
		


	return true;
}

bool SharpenPlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *SharpenPlugin::createDependency(const QString &name)
{
	Q_UNUSED(name);
	return NULL;
}

QList<QString> SharpenPlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> SharpenPlugin::toolWidgets()
{
	return QList<QWidget*>();
}

void SharpenPlugin::toolWidgetCreated(QWidget *uiWidget)
{
    Q_UNUSED(uiWidget);
}

//========================================================================================================
void SharpenPlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{
    Q_UNUSED(options);
    Q_UNUSED(changed);
    Q_UNUSED(currentLayer);
    
    if(options.options(currentLayer) == NULL) return;
}

void SharpenPlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
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
	    changes.contains(USMFilter::RadiusUSM, m_groupId)
	    || changes.contains(USMFilter::AmountUSM, m_groupId)
	    || changes.contains(USMFilter::ThresholdUSM, m_groupId)
	    || changes.contains(USMFilter::USMasClarity, m_groupId)
	){
        changes.setBool(USMFilter::EnableUSM, m_groupId, true);
	}
	
	// Sharpen1 settings changes   
	if(
	    changes.contains(WaveletSharpenFilter::Amount, m_groupId)
	    || changes.contains(WaveletSharpenFilter::Radius, m_groupId)
//	    || changes.contains(WaveletSharpenFilter::Blend, m_groupId)
	    || changes.contains(WaveletSharpenFilter::DePepper, m_groupId)
	    || changes.contains(WaveletSharpenFilter::Edges, m_groupId)
	){
        changes.setBool(WaveletSharpenFilter::Enable, m_groupId, true);
	}
	
	// Sharpen2 settings changes   
	if(
	    changes.contains(WaveletSharpenFilter::Amount2, m_groupId)
	    || changes.contains(WaveletSharpenFilter::Radius2, m_groupId)
//	    || changes.contains(WaveletSharpenFilter::Blend2, m_groupId)
	    || changes.contains(WaveletSharpenFilter::DePepper2, m_groupId)
	    || changes.contains(WaveletSharpenFilter::Edges2, m_groupId)
	){
        changes.setBool(WaveletSharpenFilter::Enable2, m_groupId, true);
	}
	
	// LLorens (Gradient) settings changes   
	if(
	    changes.contains(LLorensFilter::LLStrength, m_groupId)
	    || changes.contains(LLorensFilter::LLIter, m_groupId)
	    || changes.contains(LLorensFilter::MCStrength, m_groupId)
	){
        changes.setBool(LLorensFilter::EnableLL, m_groupId, true);
	}
}

//========================================================================================================
void SharpenPlugin::handleHotnessChanged(const PluginImageSettings &options)
{
    Q_UNUSED(options);
}

