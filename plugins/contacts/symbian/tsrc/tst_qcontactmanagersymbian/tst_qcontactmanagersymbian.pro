TEMPLATE=app
TARGET=tst_qcontactmanagersymbian

QT += testlib
CONFIG += qtestlib

symbian: {
    INCLUDEPATH += .\inc
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    SOURCES  += tst_qcontactmanagersymbian.cpp
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lqtcontacts
}
