set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin
tst_qvaluespaceitem.exe
tst_qvaluespaceobject.exe

cd %SAVED_PWD%

