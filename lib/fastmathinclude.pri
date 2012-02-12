unix {
!mac {
	unix:QMAKE_CFLAGS +=  -ffast-math 
	unix:QMAKE_CXXFLAGS +=  -ffast-math
	unix:QMAKE_LFLAGS +=  -ffast-math 
}
}



unix {
!mac {
	unix:QMAKE_CFLAGS +=  -ffinite-math-only -fno-trapping-math -fno-signaling-nans -fno-rounding-math -funsafe-math-optimizations -funsafe-loop-optimizations -fsingle-precision-constant 
	unix:QMAKE_CXXFLAGS += -ffinite-math-only -fno-trapping-math -fno-signaling-nans -fno-rounding-math -funsafe-math-optimizations -funsafe-loop-optimizations -fsingle-precision-constant
	unix:QMAKE_LFLAGS += -ffinite-math-only -fno-trapping-math -fno-signaling-nans -fno-rounding-math -funsafe-math-optimizations -funsafe-loop-optimizations -fsingle-precision-constant 
}
}


#fast math
windows {
    QMAKE_CXXFLAGS += /fp:fast
}
