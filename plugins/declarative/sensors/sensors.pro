INCLUDEPATH += ../../../src/sensors
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_sensors)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/sensors
PLUGIN_TYPE = declarative
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

#HEADERS += qdeclarativeservice.h

SOURCES += sensors.cpp 

CONFIG += mobility
MOBILITY += sensors

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

