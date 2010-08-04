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

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

HEADERS += \
        qdeclarativegalleryfilter.h \
        qdeclarativegalleryitem.h \
        qdeclarativegalleryquerymodel.h \
        qdeclarativegallerytype.h

SOURCES += \
        qdeclarativegallery.cpp \
        qdeclarativegalleryfilter.cpp \
        qdeclarativegalleryitem.cpp \
        qdeclarativegalleryquerymodel.cpp \
        qdeclarativegallerytype.cpp

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

# The symbian UID reserved for this plug-in is 0x2002BFCC
