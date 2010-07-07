INCLUDEPATH += ../../../src/telephony
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_telephony)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/telephony
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativetelephony.h

SOURCES += qdeclarativetelephony.cpp \
           telephony.cpp \

CONFIG += mobility
MOBILITY += telephony

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

