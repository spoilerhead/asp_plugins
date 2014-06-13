#!/bin/bash
export PATH=../../QT/current/Desktop/Qt/474/gcc/bin:$PATH

#make clean
make clean
#build 64bit spec

qmake -spec ../../QT/current/Desktop/Qt/474/gcc/mkspecs/linux-g++-64

#do the build
make -j8
