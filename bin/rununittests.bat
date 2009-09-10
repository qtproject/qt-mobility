set SAVED_PWD="%CD%"
set BATCH_PATH=%~dp0

set PATH=%BATCH_PATH%..\build\Debug\bin;%PATH%

cd %BATCH_PATH%..\build\tests\Debug\bin

::ServiceFramework
tst_servicemetadata.exe
tst_servicedatabase.exe
tst_databasemanager.exe
tst_qservicemanager.exe
tst_qservicefilter.exe
tst_qserviceinterfacedescriptor.exe
tst_qabstractsecuritysession.exe

::BearerManagement
tst_qnetworkconfiguration.exe
tst_qnetworkconfigurationmanager.exe
tst_qnetworksession.exe

::Location
tst_qgeocoordinate.exe
tst_qgeopositioninfo.exe
tst_qsatelliteinfo.exe
tst_qsatelliteinfosource.exe
tst_qgeopositioninfosource.exe
tst_qgeoareamonitor.exe
tst_qlocationutils.exe
tst_qnmeapositioninfosource.exe

::Publish and Subscribe
tst_qmallocpool.exe
tst_qpacket.exe
tst_qpacketprotocol.exe
tst_qvaluespace.exe
tst_qvaluespaceobject.exe
tst_qvaluespaceitem.exe
tst_qsystemreadwritelock.exe
tst_qsystemreadwritelock_oop.exe

cd %SAVED_PWD%

