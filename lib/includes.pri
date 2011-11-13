TEMPLATE = lib
TARGET = 
DEPENDPATH += .

!mac {
    INCLUDEPATH += . ../../SDK/current/Plugin ../lib
}
mac {
    INCLUDEPATH += . ../../Plugin ../lib ../newSDK
    include ( ../common/mac.pri )
}
unix {
!mac {
	unix:QMAKE_CFLAGS +=  -fPIC -Wall -O2  -mfpmath=sse -mtune=amdfam10 -msse2 -pipe
	unix:QMAKE_CXXFLAGS +=  -fPIC -Wall -O2  -mfpmath=sse -mtune=amdfam10 -msse2  -pipe
	unix:QMAKE_LFLAGS +=  -fPIC -Wall -O2  -mfpmath=sse -mtune=amdfam10 -msse2 -L /usr/lib32 -pipe
	CONFIG += x86
	unix:LIBS += -L/usr/lib32
}
}

#include(../SDK/current/examples/common/mac.pri)

#macx {
#
#	SOURCES += "Release (Universal).xcconfig"
#        QMAKE_CXXFLAGS += -faltivec
#        QMAKE_CFLAGS += -faltivec
#        QMAKE_CFLAGS_WARN_ON = -Wswitch -Wmissing-braces -Wreturn-type -Wparentheses
#        QMAKE_CFLAGS_WARN_ON += -Wunused-label -Wunused-value -Wunused-variable

#       QMAKE_CXXFLAGS_WARN_ON = $$QMAKE_CFLAGS_WARN_ON -Wnon-virtual-dtor -Woverloaded-virtual
#   	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
#   	QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk

#}

#jknights
macx {
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
	QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk
	
	
}

CONFIG(debug,debug|release) {
	message( debug )

	UI_DIR		=	builddir/objects/debug/ui
	MOC_DIR		=	builddir/objects/debug/moc
	OBJECTS_DIR	=	builddir/objects/debug/obj
	RCC_DIR	    =	builddir/objects/debug/rcc
	UI_HEADERS_DIR = builddir/objects/debug/uih
	UI_SOURCES_DIR = builddir/objects/debug/uisrc
}

CONFIG(release,debug|release) {
	message( release )

	UI_DIR		=	builddir/objects/release/ui
	MOC_DIR		=	builddir/objects/release/moc
	OBJECTS_DIR	=	builddir/objects/release/obj
	RCC_DIR	    =	builddir/objects/release/rcc
	UI_HEADERS_DIR = builddir/objects/release/uih
	UI_SOURCES_DIR = builddir/objects/release/uisrc
}
