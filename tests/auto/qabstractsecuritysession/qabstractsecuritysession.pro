TARGET=tst_qabstractsecuritysession
INCLUDEPATH += ../../../serviceframework

CONFIG+=testcase

QT = core

include(../../../common.pri)

HEADERS += ../qsfwtestutil.h
SOURCES += tst_qabstractsecuritysession.cpp \
           ../qsfwtestutil.cpp

LIBS += -lQtServiceFramework

symbian {
    TARGET.CAPABILITY = ALL -TCB
    
    addFiles.sources = ../../testservice2/xml/testserviceplugin.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles
    LIBS += -lefsrv
}