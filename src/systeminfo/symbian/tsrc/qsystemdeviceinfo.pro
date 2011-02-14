TEMPLATE = app
TARGET = tst_qsystemdeviceinfo
QT += testlib
CONFIG += qtestlib
include(../../../../common.pri)
symbian:
{
    INCLUDEPATH += ../../../
    SOURCES  += tst_qsystemdeviceinfo.cpp
    CONFIG += mobility
    MOBILITY = systeminfo
    TARGET.CAPABILITY = All -TCB -DRM
    LIBS += -lprofileengine \
            -letel3rdparty \
            -lsysutil \
            -lcentralrepository \
            -lcenrepnotifhandler \
            -lefsrv \
            -lfeatdiscovery \
            -lhwrmvibraclient \
            -lavkon \
            -lcone \
            -lws32 \
            -lcentralrepository \
            -lprofileengine \
            -lbluetooth \
            -lgdi \
            -lecom \
            -lplatformenv \
            -lhwrmlightclient \
            -lhwrmfmtxclient \
            -ldisknotifyhandler \
            -lusbman \
            -lprofileeng

            contains(S60_VERSION, 5.1) | contains(S60_VERSION, 5.2) {
            LIBS += -lhwrmpowerclient
        }
}