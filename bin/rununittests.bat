set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin
tst_qvaluespace.exe
tst_qvaluespaceprovider.exe
tst_qvaluespacesubscriber.exe
tst_qvaluespacesubscriber_oop.exe
tst_qsystemreadwritelock.exe
tst_qsystemreadwritelock_oop.exe
tst_qcrmlparser.exe

cd %SAVED_PWD%

