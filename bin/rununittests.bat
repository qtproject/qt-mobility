set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin
tst_qmallocpool.exe
tst_qpacket.exe
tst_qpacketprotocol.exe
tst_qvaluespace.exe
tst_qvaluespaceobject.exe
tst_qvaluespaceitem.exe
tst_qvaluespaceitem_oop.exe

cd %SAVED_PWD%

