SOURCES += tst_qservicefilter.cpp
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework
CONFIG+=testcase

QT = core

include(../../../common.pri)

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
