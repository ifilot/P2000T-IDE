@ECHO OFF

rmdir /S bin

REM *************
REM Release build
REM *************
@ECHO ********************
@ECHO Deploying P2000T-IDE
@ECHO ********************

REM Create directories
mkdir bin

REM Copy files Managlyph
copy build\release\release\p2000t-ide.exe bin
copy assets\images\p2000t-ide.ico bin\p2000t-ide.ico

set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC
set VERSION=5.15.2
set EXECUTABLE=p2000t-ide.exe
set APPVERSION=0.10.1

if exist C:\Qt\%VERSION%\msvc2019_64\bin\windeployqt.exe (
    C:\Qt\%VERSION%\msvc2019_64\bin\windeployqt.exe bin\%EXECUTABLE% --release --force --compiler-runtime
)
if exist D:\Qt\%VERSION%\msvc2019_64\bin\windeployqt.exe (
    D:\Qt\%VERSION%\msvc2019_64\bin\windeployqt.exe bin\%EXECUTABLE% --release --force --compiler-runtime
)

REM Build installer
"C:\Program Files (x86)\NSIS\Bin\makensis.exe" p2000t-ide.nsi

rename p2000t-ide_installer_win64.exe p2000t-ide_installer_win64_%APPVERSION%.exe
powershell Compress-Archive p2000t-ide_installer_win64_%APPVERSION%.exe p2000t-ide_installer_win64_%APPVERSION%.zip

pause
