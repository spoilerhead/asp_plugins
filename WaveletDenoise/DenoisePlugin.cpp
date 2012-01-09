#include "DenoisePlugin.h"
#include "DenoiseFilter.h"
#include "PluginHub.h"
#include "PluginDependency.h"

#include <QDebug>
#include <iostream>
#include <QSettings>

#define PLUGIN_NAME_HR "Wavelet Denoise 2"

extern "C" BIBBLE_API BaseB5Plugin *b5plugin() { return new DenoisePlugin; }


bool DenoisePlugin::init(PluginHub *hub, int id, int groupId, const QString &bundlePath)
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

bool DenoisePlugin::registerFilters()
{
	DenoiseFilter *Denoiser = new DenoiseFilter(m_hub, m_groupId);
	//if (m_hub->addFilter(mirror, PluginHub::After, QString("RgbToLab"))) {
//	if (m_hub->addFilter(Denoiser, PluginHub::After, QString("RgbToLab"))) {
    if (m_hub->addFilter(Denoiser, PluginHub::After, QString("CameraRgbToWorkingspace_Raw"))) {
        qDebug() << "Successfully registered "<<PLUGIN_NAME_HR<<" filter";
		return true;
	}
	qDebug() << "Failed to register "<<PLUGIN_NAME_HR<<" filter";
	return false;
}

bool DenoisePlugin::registerOptions()
{
	m_hub->addBoolOption  (DenoiseFilter::Enable,  "bSphWaveletDenoiseon"           , "Denoise Enable",  "Enable Filter", "WaveletDenoise2Filter", false, false, 0);
	m_hub->addDoubleOption   (DenoiseFilter::TresL,   "bSphWaveletDenoiseThresL"    , "Denoise Threshold Luminance",  "Denoiseing Strength, L-Channel (Luminance)", "WaveletDenoise2Filter", 0, 0, 0);
	m_hub->addDoubleOption   (DenoiseFilter::LowL,    "bSphWaveletDenoiseLowL"      , "Denoise Detail Luminance",  "Detail Conservation, L-Channel (Luminance)", "WaveletDenoise2Filter", 0, 0, 0);

  	m_hub->addDoubleOption   (DenoiseFilter::TresC,   "bSphWaveletDenoiseThresC"    , "Denoise Threshold Color",  "Denoiseing Strength, A/B-Channels (color)", "WaveletDenoise2Filter", 0, 0, 0);
	m_hub->addDoubleOption   (DenoiseFilter::LowC,    "bSphWaveletDenoiseLowC"      , "Denoise Detail Color",  "Detail Conservation, A/B-Channels (color)", "WaveletDenoise2Filter", 0, 0, 0);	

	m_hub->addBoolOption  (DenoiseFilter::DePepper, "bSphWaveletDenoiseDePepper"    , "Denoise DePepper", "Salt and Pepper noise removal (slow)", "WaveletDenoise2Filter", false, false, 0);
    m_hub->addBoolOption  (DenoiseFilter::ShowEdges, "bSphWaveletDenoiseShowEdge"    , "Denoise Show Edges", "Show Edge Gradients used", "WaveletDenoise2Filter", false, false, 0);
	
	return true;
}

bool DenoisePlugin::finish()
{
	connect(m_hub, SIGNAL(settingsChanged(const PluginImageSettings &, const PluginImageSettings &, int)), this, SLOT(handleSettingsChange(const PluginImageSettings &, const PluginImageSettings &, int)));
	connect(m_hub, SIGNAL(controlChanged(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)), this, SLOT(handleControlChange(const QString &, int, int , const PluginImageSettings &, const PluginOptionList &, PluginOptionList &)));
   	connect(m_hub, SIGNAL(hotnessChanged(const PluginImageSettings &)), this, SLOT(handleHotnessChanged(const PluginImageSettings &)));
	return true;
}

PluginDependency *DenoisePlugin::createDependency(const QString &name)
{
	Q_UNUSED(name);
	return NULL;
}

QList<QString> DenoisePlugin::toolFiles()
{
	return QList<QString>();
}

QList<QWidget*> DenoisePlugin::toolWidgets()
{
	return QList<QWidget*>();
}

void DenoisePlugin::toolWidgetCreated(QWidget *uiWidget)
{
}


//========================================================================================================
void DenoisePlugin::handleSettingsChange(const PluginImageSettings &options, const PluginImageSettings &changed, int currentLayer)
{

    if(options.options(currentLayer) == NULL) return;
}

void DenoisePlugin::handleControlChange(const QString &optionName, int groupId, int layer, const PluginImageSettings &options, const PluginOptionList &current, PluginOptionList &changes)
{
    //   v-- You need to add this if you're going to use the options --v
    //   some changes do not have options - check for NULL
    if (options.options(layer) == NULL) return;
    
    // Denoise settings changes   
	if(
	    changes.contains(DenoiseFilter::TresL, m_groupId)
	    || changes.contains(DenoiseFilter::LowL, m_groupId)
	    || changes.contains(DenoiseFilter::TresC, m_groupId)
	    || changes.contains(DenoiseFilter::LowC, m_groupId)
	    || changes.contains(DenoiseFilter::DePepper, m_groupId)
	){
        changes.setBool(DenoiseFilter::Enable, m_groupId, true);
	}
}

//========================================================================================================
void DenoisePlugin::handleHotnessChanged(const PluginImageSettings &options)
{
}
