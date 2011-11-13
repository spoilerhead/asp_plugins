#ifndef _SPH_H
#define _SPH_H
#include <string>
#include <cmath>

#include "BibblePlugin.h"
#include "wavelet.h"

using namespace std;

#if defined( __GNUC__ ) && ( __GNUC__ >= 4 )
#	define __declspec( a ) __attribute__( ( visibility( "default" ) ) )
#elif !defined( WIN32 )
#	define __declspec( a )
#endif

// Export functions to register the plug-in with Bibble
extern "C" {
__declspec(dllexport) int unRegisterPlugin(struct BibblePluginData *bibbleData);
__declspec(dllexport) int RegisterPlugin(struct BibblePluginData *bibbleData);
__declspec(dllexport) int getVersion( void );
__declspec(dllexport) bool runCheck(struct BibblePluginData *bibbleData, struct BPluginPipeData *pipeData, BBOOL &needPrev );
__declspec(dllexport) int buildDefaults(struct BibblePluginData *bibbleData, struct BAddOptionFunctions *baf );
}



		     

// Define the settings strings - these must match the prefixes on the controls
// for the option system to work properly
#define WAVELET_ENABLED	21  // Boolean - is the plug-in enabled?
#define WAVELET_TRES_L	22
#define WAVELET_LOW_L	23
#define WAVELET_TRES_A	24
#define WAVELET_LOW_A	25
#define WAVELET_TRES_B	26
#define WAVELET_LOW_B	27
#define WAVELET_DEPEPPER 28

#define WAVELETDENOISE_PLUGINNAME	"BSphWaveletDenoisePlugin"
#define WAVELETDENOISE_PLUGINGROUP	"sphPlugins"// The  plugin group/owner name  for all of yout plug-in's settings keep it short but unique
#define WAVELETDENOISE_GROUP		"Wavelet Denoise Plugin"		// The group for all of this plug-in's settings

// Grab a pointer to the debug trace function and define a macro to simplify using it
typedef void (*BDebugTraceFunction)(const char *msg);
extern BDebugTraceFunction BTestDebug;
#define BMSG(A)  if (BTestDebug) { BTestDebug(A); } 

#endif // _WAVELETDENOISE_H

