INCLUDEPATH += ../../../src/serviceframework
INCLUDEPATH += ../../../src/global

TARGET  = declarative_serviceframework
TARGETPATH = QtMobility/serviceframework
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativeservice.h

SOURCES += qdeclarativeservice.cpp \
           serviceframework.cpp \

CONFIG += mobility
MOBILITY += serviceframework

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

