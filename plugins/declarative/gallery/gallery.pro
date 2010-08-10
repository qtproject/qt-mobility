DEPENDPATH += .
INCLUDEPATH += . \
    ../../../include \
    ../../../src/gallery

TARGET = $$qtLibraryTarget(declarative_gallery)
TARGETPATH = QtMobility/gallery

TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE = declarative

include(../../../common.pri)

QT += declarative


CONFIG += mobility
MOBILITY = gallery

HEADERS += \
        qdeclarativegalleryfilter.h \
        qdeclarativegalleryitem.h \
        qdeclarativegalleryquerycount.h \
        qdeclarativegalleryquerymodel.h

SOURCES += \
        qdeclarativegallery.cpp \
        qdeclarativegalleryfilter.cpp \
        qdeclarativegalleryitem.cpp \
        qdeclarativegalleryquerycount.cpp \
        qdeclarativegalleryquerymodel.cpp

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
    TARGET.UID3 = 0x2002BFCC
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_gallery$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }

