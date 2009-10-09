set DEPOT="%CD%"

IF NOT EXIST bearermanagement.pro GOTO NOSOURCE
IF EXIST %QTDIR%\.qmake.cache set CETESTCOMMAND=cetest -cache %QTDIR%\.qmake.cache -libpath \Windows 
IF NOT EXIST %QTDIR%\.qmake.cache set CETESTCOMMAND=cetest -libpath \Windows 

IF NOT EXIST %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\ GOTO SKIPDEPLOY
cd %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\
call %CETESTCOMMAND%  

:SKIPDEPLOY
cd %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\qnetworkconfiguration
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\qnetworkconfigmanager
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\qnetworksession
GOTO EOF

:NOSOURCE
echo "Unable to find bearermanagement.pro check current directory is bearermanagment source" 
GOTO EOF

:EOF
cd %DEPOT%

