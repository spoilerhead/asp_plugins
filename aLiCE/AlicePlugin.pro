######################################################################
# Automatically generated by qmake (2.01a) Di. Jun 8 19:18:33 2010
######################################################################
TARGET = Alice
VERSION = 0.1
include(../lib/includes.pri)
include(../lib/fastmathinclude.pri)


# Input
HEADERS += FilterPyramid.h AlicePlugin.h ../lib/PluginTools/ToolData.h
TRANSLATIONS = AlicePlugin_de.ts AlicePlugin_fr.ts AlicePlugin_it.ts AlicePlugin_ja.ts AlicePlugin_nl.ts

FORMS += alice.ui
SOURCES += AlicePlugin.cpp    
SOURCES += ../lib/sphLayerFilter.cpp 
SOURCES += FilterPyramid.cpp
SOURCES += ../lib/usm_iir.cpp
#SOURCES += ../lib/convolution.cpp
SOURCES += ../lib/PluginTools/ToolData.cpp

