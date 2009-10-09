set DEPOT="%CD%"

IF NOT EXIST multimedia.pro GOTO NOSOURCE
IF EXIST %QTDIR%\.qmake.cache set CETESTCOMMAND=cetest -cache %QTDIR%\.qmake.cache -libpath \Windows 
IF NOT EXIST %QTDIR%\.qmake.cache set CETESTCOMMAND=cetest -libpath \Windows 

IF NOT EXIST %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\ GOTO SKIPDEPLOY
cd %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\
call %CETESTCOMMAND%  

:SKIPDEPLOY
cd %DEPOT%\tests\auto\qaudiocapturesource
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcamera
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qgraphicsvideoitem
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediacontent
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaimageviewer
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaobject
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaplayer
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaplaylist
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaplaylistnavigator
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediapluginloader
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediarecorder
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaresource
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaservice
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qmediaserviceprovider
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qradiotuner
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qvideowidget
call %CETESTCOMMAND%
GOTO EOF

:NOSOURCE
echo "Unable to find multimedia.pro check current directory is multimedia source" 
GOTO EOF

:EOF
cd %DEPOT%

