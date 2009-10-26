set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin
tst_qnetworkconfiguration.exe
tst_qnetworkconfigurationmanager.exe
tst_qnetworksession.exe

cd %SAVED_PWD%

