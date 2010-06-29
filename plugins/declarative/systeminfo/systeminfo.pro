INCLUDEPATH += ../../../src/systeminfo
DEPENDPATH += ../../../src/systeminfo
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_systeminfo)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/systeminfo
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

SOURCES += systeminfo.cpp

CONFIG += mobility
MOBILITY += systeminfo

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

