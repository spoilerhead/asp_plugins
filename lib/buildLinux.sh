#!/bin/bash
export PATH=../../QT/current/Desktop/Qt/474/gcc/bin:$PATH
echo $PATH
#make clean
make clean
#build 32bit spec
#qmake -spec ../../QT/current/mkspecs/linux-g++-32
qmake -spec ../../QT/current/Desktop/Qt/474/gcc/mkspecs/linux-g++-32
#replace system QT with my QT verson (ASP uses 4.7.*)
#sed -i -e "s#/usr/include/qt4#../../QT/current/include#g" Makefile

#sed -i -e "s#-L/usr/lib/x86_64-linux-gnu#-L/opt/AfterShotPro/lib  -L/usr/lib/x86_64-linux-gnu#g" Makefile
#do the build
make
