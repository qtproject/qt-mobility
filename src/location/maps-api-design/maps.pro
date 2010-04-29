TEMPLATE = lib
TARGET = QtMaps

QT = core gui network

include(../../../common.pri)

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += . ../
DEPENDPATH += .

CONFIG += mobility
MOBILITY += location

PUBLIC_HEADERS += \
    qgeoboundingbox.h \
    qgeocodereply.h \
    qgeocoderequestoptions.h \
    qgeocodingservice.h \
    qgeomapobject.h \
    qgeomappingservice.h \
    qgeomapreply.h \
    qgeomapwidget.h \
    qgeonavigationhelper.h \
    qgeonavigationinstruction.h \
    qgeoroute.h \
    qgeoroutereply.h \
    qgeorouterequestoptions.h \
    qgeoroutesegment.h \
    qgeoroutingservice.h

PRIVATE_HEADERS += \
    qgeoboundingbox_p.h \
    qgeocoderequestoptions_p.h \
    qgeomapobject_p.h \
    qgeomapwidget_p.h \
    qgeonavigationhelper_p.h \
    qgeonavigationinstruction_p.h \
    qgeoroute_p.h \
    qgeorouterequestoptions_p.h \
    qgeoroutesegment_p.h

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
    qgeoboundingbox.cpp \
    qgeocodereply.cpp \
    qgeocoderequestoptions.cpp \
    qgeocodingservice.cpp \
    qgeomapobject.cpp \
    qgeomappingservice.cpp \
    qgeomapreply.cpp \
    qgeomapwidget.cpp \
    qgeonavigationhelper.cpp \
    qgeonavigationinstruction.cpp \
    qgeoroute.cpp \
    qgeoroutereply.cpp \
    qgeorouterequestoptions.cpp \
    qgeoroutesegment.cpp \
    qgeoroutingservice.cpp

CONFIG += middleware
include(../../../features/deploy.pri)

