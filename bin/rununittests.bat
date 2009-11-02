set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin
tst_qsystemdeviceinfo.exe
tst_qsystemdisplayinfo.exe
tst_qsysteminfo.exe
tst_qsystemnetworkinfo.exe
tst_qsystemscreensaver.exe
tst_qsystemstorageinfo.exe


cd %SAVED_PWD%

