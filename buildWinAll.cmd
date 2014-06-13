call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
cd aLiCE
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
cd ..\FatToni
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
cd ..\Gelatin
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
cd ..\Grain
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
cd ..\SiliconBonk
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
cd ..\WaveletDenoise
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
cd ..\WaveletSharpen
call buildwin
msbuild /t:Rebuild /p:Configuration=Release
pause