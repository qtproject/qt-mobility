set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin
tst_servicemetadata.exe
tst_servicedatabase.exe
tst_databasemanager.exe
tst_qservicemanager.exe
tst_qservicefilter.exe
tst_qserviceinterfacedescriptor.exe
tst_qabstractsecuritysession.exe


cd %SAVED_PWD%

