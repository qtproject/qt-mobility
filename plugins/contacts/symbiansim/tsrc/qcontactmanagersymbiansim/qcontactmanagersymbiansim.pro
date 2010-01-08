TEMPLATE=app
TARGET=tst_qcontactmanagersymbiansim

QT += testlib
CONFIG += qtestlib

symbian: {
    INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    SOURCES  += tst_qcontactmanagersymbiansim.cpp
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lqtcontacts
}
