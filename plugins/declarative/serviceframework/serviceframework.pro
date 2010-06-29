INCLUDEPATH += ../../../src/serviceframework
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_serviceframework)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/serviceframework
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativeservice.h

SOURCES += qdeclarativeservice.cpp \
           serviceframework.cpp 

CONFIG += mobility
MOBILITY += serviceframework

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

symbian:
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    load(armcc_warnings)
