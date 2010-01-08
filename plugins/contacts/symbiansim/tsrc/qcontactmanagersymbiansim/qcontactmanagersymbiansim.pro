QT += testlib
TEMPLATE=app
TARGET=tst_qcontactmanagersymbiansim
CONFIG += testcase
#CONFIG += qtestlib
CONFIG += mobility
MOBILITY = contacts
SOURCES  += tst_qcontactmanagersymbiansim.cpp

symbian: {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles
    LIBS += -lqtcontacts
}
