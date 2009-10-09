set DEPOT="%CD%"

IF NOT EXIST sfw.pro GOTO NOSOURCE
set CETESTCOMMAND=cetest -cache %QTDIR%\.qmake.cache -libpath \Windows 

IF NOT EXIST %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\ GOTO SKIPDEPLOY
cd %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\
call %CETESTCOMMAND%  

:SKIPDEPLOY
cd %DEPOT%\tests\auto\servicemetadata
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\servicedatabase
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\databasemanager
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\qservicemanager"
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qservicefilter
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\qserviceinterfacedescriptor
call %CETESTCOMMAND%  
cd %DEPOT%\tests\auto\qabstractsecuritysession
call %CETESTCOMMAND%  
GOTO EOF

:NOSOURCE
echo "Unable to find sfw.pro check current directory is serviceframework source" 
GOTO EOF

:EOF
cd %DEPOT%

