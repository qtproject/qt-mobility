set DEPOT="%CD%"

IF NOT EXIST mobility.pro GOTO NOSOURCE
IF EXIST %QTDIR%\.qmake.cache set CETESTCOMMAND=cetest -cache %QTDIR%\.qmake.cache -libpath \Windows 
IF NOT EXIST %QTDIR%\.qmake.cache set CETESTCOMMAND=cetest -libpath \Windows 

IF NOT EXIST %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\ GOTO SKIPDEPLOY
cd %DEPOT%\..\..\qa-dungeon\mainline\tests\wince\deploy\
call %CETESTCOMMAND%  

:SKIPDEPLOY
cd %DEPOT%\tests\auto\qcontact
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactactions\unittest
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactasync\unittest
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactdetail
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactdetaildefinition
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactdetails
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactfilter
call %CETESTCOMMAND%
cd %DEPOT%\testsauto\qcontactgroup
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactmanager
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactmanagerfiltering
call %CETESTCOMMAND%
cd %DEPOT%\tests\auto\qcontactmanagerplugins\unittest
call %CETESTCOMMAND%
GOTO EOF

:NOSOURCE
echo "Unable to find mobility.pro check current directory is QtContacts source" 
GOTO EOF

:EOF
cd %DEPOT%

