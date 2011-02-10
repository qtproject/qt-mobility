INCLUDEPATH += ../../../src/systeminfo
DEPENDPATH += ../../../src/systeminfo
INCLUDEPATH += ../../../src/global

TEMPLATE = lib
CONFIG += plugin
TARGET  = $$qtLibraryTarget(declarative_systeminfo)
TARGETPATH = QtMobility/systeminfo
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

SOURCES += systeminfo.cpp \
    qdeclarativenetworkinfo.cpp \
    qdeclarativescreensaver.cpp \
    qdeclarativedeviceinfo.cpp \
    qdeclarativegeneralinfo.cpp \
    qdeclarativebatteryinfo.cpp \
    qdeclarativealignedtimer.cpp \
    qdeclarativedisplayinfo.cpp

HEADERS += \
    qdeclarativenetworkinfo_p.h \
    qdeclarativescreensaver_p.h \
    qdeclarativedeviceinfo_p.h \
    qdeclarativegeneralinfo_p.h \
    qdeclarativebatteryinfo_p.h \
    qdeclarativealignedtimer_p.h \
    qdeclarativedisplayinfo_p.h

CONFIG += mobility
MOBILITY += systeminfo

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x200315F9
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_systeminfo$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
