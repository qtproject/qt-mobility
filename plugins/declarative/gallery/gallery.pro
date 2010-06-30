INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../../src/gallery
INCLUDEPATH += ../../../src/global

TARGET  = declarative_gallery
TARGETPATH = QtMobility/gallery
include(../../qimportbase.pri)
include(../../../common.pri)

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

