:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: Contact: Nokia Corporation (qt-info@nokia.com)
::
:: This file is part of the Qt Mobility Components.
::
:: $QT_BEGIN_LICENSE:LGPL$
:: No Commercial Usage
:: This file contains pre-release code and may not be distributed.
:: You may use this file in accordance with the terms and conditions
:: contained in the Technology Preview License Agreement accompanying
:: this package.
::
:: GNU Lesser General Public License Usage
:: Alternatively, this file may be used under the terms of the GNU Lesser
:: General Public License version 2.1 as published by the Free Software
:: Foundation and appearing in the file LICENSE.LGPL included in the
:: packaging of this file.  Please review the following information to
:: ensure the GNU Lesser General Public License version 2.1 requirements
:: will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
::
:: In addition, as a special exception, Nokia gives you certain additional
:: rights.  These rights are described in the Nokia Qt LGPL Exception
:: version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
::
:: If you have questions regarding the use of this file, please contact
:: Nokia at qt-info@nokia.com.
::
::
::
::
::
::
::
::
:: $QT_END_LICENSE$
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off

set QT_MOBILITY_PREFIX= C:\QtMobility
set BUILD_PATH=%CD%
set SOURCE_PATH= %~dp0
cd %SOURCE_PATH%
set SOURCE_PATH=%CD%
cd %BUILD_PATH%

set PROJECT_CONFIG= %BUILD_PATH%\config.in
set PROJECT_LOG= %BUILD_PATH%\config.log
set RELEASEMODE=release
set QT_MOBILITY_LIB=
set BUILD_UNITTESTS=no
set BUILD_EXAMPLES=no
set CONTACTS_PLUGIN=
set VC_TEMPLATE_OPTION=
set QMAKE_CACHE=%BUILD_PATH%\.qmake.cache

if exist "%QMAKE_CACHE%" del %QMAKE_CACHE%
if exist "%PROJECT_LOG%" del %PROJECT_LOG%
if exist "%PROJECT_CONFIG%" del %PROJECT_CONFIG%
echo CONFIG += silent > %PROJECT_CONFIG%

echo QT_MOBILITY_SOURCE_TREE = %SOURCE_PATH% > %QMAKE_CACHE%
echo QT_MOBILITY_BUILD_TREE = %BUILD_PATH% >> %QMAKE_CACHE%

:cmdline_parsing

if "%1" == ""               goto startProcessing
if "%1" == "-debug"         goto debugTag
if "%1" == "-release"       goto releaseTag
if "%1" == "-prefix"        goto prefixTag
if "%1" == "-libdir"        goto libTag
if "%1" == "-bindir"        goto binTag
if "%1" == "-headerdir"     goto headerTag
if "%1" == "-tests"         goto testTag
if "%1" == "-examples"      goto exampleTag
if "%1" == "-contact-src"   goto contactsTag
if "%1" == "-vc"            goto vcTag
if "%1" == "/?"             goto usage
if "%1" == "-h"             goto usage
if "%1" == "-help"          goto usage
if "%1" == "--help"         goto usage


echo Unknown option: "%1"
goto usage

:usage
echo Usage: configure.bat [-prefix (dir)] [headerdir (dir)] [libdir (dir)]
    echo                  [-bindir (dir)] [-tests] [-examples] [-debug] [-release]
    echo.
    echo Options:
    echo.
    echo -prefix (dir) ..... This will install everything relative to dir
    echo                     (default prefix: C:\QtMobility)
    echo -headerdir (dir) .. Header files will be installed to dir
    echo                     (default prefix: PREFIX/include)
    echo -libdir (dir) ..... Libraries will be installed to dir
    echo                     (default PREFIX/lib)
    echo -bindir (dir) ..... Executables will be installed to dir
    echo                     (default PREFIX/bin)
    echo -debug ............ Build with debugging symbols
    echo -release .......... Build without debugging symbols
    echo -tests ............ Build unit tests (not build by default)
    echo                     Note, this adds test symbols to all libraries 
    echo                     and should not be used for release builds.
    echo -examples ......... Build example applications
    echo -vc ............... Generate Visual Studio make files
    echo -contact-src (backend) ...
    echo                     Compile the specified contacts API backend. Not selecting any backend
    echo                     will result in default selection for build platform
    echo                     options: symbian, wince, memory, maemo


if exist "%PROJECT_CONFIF%" del %PROJECT_CONFIG%
goto exitTag

:contactsTag
shift
set CONTACTS_PLUGIN=%CONTACTS_PLUGIN% %1
shift
goto cmdline_parsing

:debugTag
if "%RELEASEMODE%" == "release" set RELEASEMODE=debug
shift
goto cmdline_parsing

:releaseTag
if "%RELEASEMODE%" == "debug"   set RELEASEMODE=release
shift
goto cmdline_parsing

:prefixTag
shift
set QT_MOBILITY_PREFIX=%1
shift
goto cmdline_parsing

:libTag
shift
echo QT_MOBILITY_LIB = %1 >> %PROJECT_CONFIG%
shift
goto cmdline_parsing

:binTag
shift
echo QT_MOBILITY_BIN = %1 >> %PROJECT_CONFIG%
shift
goto cmdline_parsing

:headerTag
shift
echo QT_MOBILITY_INCLUDE = %1 >> %PROJECT_CONFIG%
shift
goto cmdline_parsing

:testTag
set BUILD_UNITTESTS=yes
shift
goto cmdline_parsing

:exampleTag
set BUILD_EXAMPLES=yes
shift
goto cmdline_parsing

:vcTag
shift
set VC_TEMPLATE_OPTION=-tp vc
goto cmdline_parsing

:startProcessing

echo CONFIG += %RELEASEMODE% >> %PROJECT_CONFIG%
echo CONTACTS_BACKENDS = %CONTACTS_PLUGIN% >> %PROJECT_CONFIG%

set CURRENTDIR=%CD%
echo %CURRENTDIR%
if exist %QT_MOBILITY_PREFIX% goto prefixExists
mkdir %QT_MOBILITY_PREFIX%
cd %QT_MOBILITY_PREFIX%
set QT_MOBILITY_PREFIX=%CD%
cd %CURRENTDIR%
rd /S /Q %QT_MOBILITY_PREFIX%
goto endprefixProcessing

:prefixExists
cd %QT_MOBILITY_PREFIX%
set QT_MOBILITY_PREFIX=%CD%
cd %CURRENTDIR%

:endprefixProcessing
echo QT_MOBILITY_PREFIX = %QT_MOBILITY_PREFIX% >> %PROJECT_CONFIG%

echo build_unit_tests = %BUILD_UNITTESTS% >> %PROJECT_CONFIG%
echo build_examples = %BUILD_EXAMPLES% >> %PROJECT_CONFIG%
echo qmf_enabled = no >> %PROJECT_CONFIG%

echo isEmpty($$QT_MOBILITY_INCLUDE):QT_MOBILITY_INCLUDE=$$QT_MOBILITY_PREFIX/include >> %PROJECT_CONFIG%
echo isEmpty($$QT_MOBILITY_LIB):QT_MOBILITY_LIB=$$QT_MOBILITY_PREFIX/lib >> %PROJECT_CONFIG%
echo isEmpty($$QT_MOBILITY_BIN):QT_MOBILITY_BIN=$$QT_MOBILITY_PREFIX/bin >> %PROJECT_CONFIG%


copy %PROJECT_CONFIG% %BUILD_PATH%\config.pri
del %PROJECT_CONFIG%

echo Checking available Qt
qmake -v >> %PROJECT_LOG% 2>&1
if errorlevel 1 goto qmakeNotFound
goto qmakeFound
:qmakeNotFound
echo ... Not found  >> %PROJECT_LOG% 2>&1
echo >&2 "Cannot find 'qmake' in your PATH."
echo >&2 "Aborting." 
goto exitTag

:qmakeFound
qmake -query QT_VERSION

echo Checking for nmake...
nmake /? >> %PROJECT_LOG% 2>&1
if errorlevel 1 goto mingw
echo       Using nmake
set MAKE=nmake
goto testingMake

:mingw
echo Checking for mingw32-make...
mingw32-make -v >> %PROJECT_LOG% 2>&1
if errorlevel 1 goto gnumake
echo       Using mingw32-make
set MAKE=mingw32-make
goto testingMake

:gnumake
echo Checking for GNU make...
make -v >> %PROJECT_LOG% 2>&1
if errorlevel 1 goto gnumake
echo       Using GNU make
set MAKE=make
goto testingMake

echo >&2 "Cannot find 'nmake', 'mingw32-make' or 'make' in your PATH"
echo >&2 "Aborting."

goto exitTag

:testingMake

echo.
REM compile tests go here. We don't have anything to test for at this stage.

if not exist "%BUILD_PATH%\features" mkdir %BUILD_PATH%\features
echo "Generating Mobility Headers..."
rd /s /q %BUILD_PATH%\include
mkdir %BUILD_PATH%\include
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\bearer
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\context
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\location
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\serviceframework
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\systeminfo
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\contacts
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\multimedia
perl -S %SOURCE_PATH%\bin\syncheaders %BUILD_PATH%\include %SOURCE_PATH%\messaging

echo.
echo Running qmake...
qmake -recursive %VC_TEMPLATE_OPTION% %SOURCE_PATH%\qtmobility.pro
if errorlevel 1 goto qmakeRecError
echo.
echo configure has finished. You may run %MAKE% to build the project now.
goto exitTag

:qmakeRecError
echo.
echo configure failed.

:exitTag
