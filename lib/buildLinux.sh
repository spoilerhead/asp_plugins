#!/bin/bash

#make clean
make clean
#build 32bit spec
qmake -spec ../../QT/current/mkspecs/linux-g++-32
#replace system QT with my QT verson (ASP uses 4.7.*)
sed -i -e "s#/usr/include/qt4#../../QT/current/include#g" Makefile
#do the build
make
