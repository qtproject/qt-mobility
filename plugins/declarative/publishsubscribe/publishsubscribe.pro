INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_publishsubscribe)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/publishsubscribe
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

SOURCES += publishsubscribe.cpp

CONFIG += mobility
MOBILITY += publishsubscribe

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

symbian:TARGET.EPOCALLOWDLLDATA=1
