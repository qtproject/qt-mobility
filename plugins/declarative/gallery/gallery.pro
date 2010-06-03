INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../../src/gallery
INCLUDEPATH += ../../../src/global

TARGET  = declarative_gallery
TARGETPATH = QtMobility/gallery
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

HEADERS += \
        galleryfilter.h \
        galleryfilterrequest.h \
        galleryitemlistmodel.h

SOURCES += \
        gallery.cpp \
        galleryfilter.cpp \
        galleryfilterrequest.cpp \
        galleryitemlistmodel.cpp

CONFIG += mobility
MOBILITY += gallery

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

