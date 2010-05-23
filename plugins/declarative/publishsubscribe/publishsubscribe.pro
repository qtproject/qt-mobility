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

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

