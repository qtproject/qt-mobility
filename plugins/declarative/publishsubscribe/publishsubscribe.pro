INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe
INCLUDEPATH += ../../../src/global

TARGET  = declarative_publishsubscribe
TARGETPATH = QtMobility/publishsubscribe
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

SOURCES += publishsubscribe.cpp

CONFIG += mobility
MOBILITY += publishsubscribe

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

