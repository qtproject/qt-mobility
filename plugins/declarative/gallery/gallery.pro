INCLUDEPATH += ../../../src/gallery
DEPENDPATH += ../../../src/gallery
INCLUDEPATH += ../../../src/global

TARGET  = declarative_gallery
TARGETPATH = QtMobility/gallery
include(../../qimportbase.pri)
include(../../../common.pri)

QT += declarative

HEADERS += \
        gallerycountrequest.h \
        galleryfilter.h \
        galleryitemrequest.h \
        galleryqueryrequest.h \
        galleryremoverequest.h \
        galleryurlrequest.h

SOURCES += \
        gallerycountrequest.cpp \
        gallery.cpp \
        galleryfilter.cpp \
        galleryitemrequest.cpp \
        galleryqueryrequest.cpp \
        galleryremoverequest.cpp \
        galleryurlrequest.cpp

CONFIG += mobility
MOBILITY += gallery

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

