INCLUDEPATH += ../../../src/systeminfo
DEPENDPATH += ../../../src/systeminfo
INCLUDEPATH += ../../../src/global

TARGET  = declarative_systeminfo
TARGETPATH = QtMobility/systeminfo
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

SOURCES += systeminfo.cpp

CONFIG += mobility
MOBILITY += systeminfo

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

