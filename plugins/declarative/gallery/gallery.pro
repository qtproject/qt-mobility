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

QT += declarative

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

CONFIG += mobility
MOBILITY += gallery

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

