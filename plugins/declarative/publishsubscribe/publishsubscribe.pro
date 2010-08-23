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

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x20021322
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_publishsubscribe$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }