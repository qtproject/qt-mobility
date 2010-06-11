INCLUDEPATH += ../../../src/telephony
INCLUDEPATH += ../../../src/global

TARGET  = declarative_telephony
TARGETPATH = QtMobility/telephony
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativetelephony.h

SOURCES += qdeclarativetelephony.cpp \
           telephony.cpp \

CONFIG += mobility
MOBILITY += telephony

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

