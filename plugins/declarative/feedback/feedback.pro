INCLUDEPATH += ../../../src/feedback
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_feedback)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/feedback
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativehapticseffect.h \
    qdeclarativefileeffect.h \
    qdeclarativethemeeffect.h \
    qdeclarativefeedback.h
SOURCES += feedback.cpp

CONFIG += mobility
MOBILITY += feedback

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
    TARGET.UID3 = 0x200315FC
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_feedback$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
