@echo off

set QT_MOBILITY_PREFIX= C:\QtMobility
set PROJECT_PWD= %~dp0
set PROJECT_CONFIG= %~dp0config.in
set RELEASEMODE=release
set QT_MOBILITY_LIB=
set BUILD_UNITTESTS=no
set BUILD_EXAMPLES=no
set CONTACTS_PLUGIN=

if exist "%PROJECT_CONFIG%" del %PROJECT_CONFIG%
echo CONFIG += silent > %PROJECT_CONFIG%

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
    echo -examples ......... Build example applications
    echo "-contact-src <backend> ..."
    echo "                   Compile the specified contacts API backend. Not selecting any backend"
    echo "                   will result in default selection for build platform"
    echo "                   options: symbian, wince, kabc, memory"


del config.in
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

copy %PROJECT_CONFIG% %PROJECT_PWD%config.pri
del %PROJECT_CONFIG%

echo "Generating Mobility Headers..."
rd /s /q %PROJECT_PWD%\include
mkdir %PROJECT_PWD%\include
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\bearer
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\context
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\location
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\serviceframework
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\systeminfo
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\contacts
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\multimedia
perl -S %PROJECT_PWD%\bin\syncheaders %PROJECT_PWD%\include %PROJECT_PWD%\messaging

echo Running qmake...
qmake -recursive

:exitTag
