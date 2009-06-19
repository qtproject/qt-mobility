TARGET=tst_qabstractsecuritysession
INCLUDEPATH += ../../../servicefw/serviceframework/serviceresolution \
               ../../../servicefw/serviceframework/servicehandler \
               ../../../servicefw/serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

HEADERS += ../qsfwtestutil.h
SOURCES += tst_qabstractsecuritysession.cpp \
           ../qsfwtestutil.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
