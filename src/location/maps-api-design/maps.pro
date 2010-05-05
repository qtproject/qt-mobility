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
    qgeodistance.h \
    qgeomapobject.h \
    qgeomappingservice.h \
    qgeomapreply.h \
    qgeomaprequestoptions.h \
    qgeomapwidget.h \
    qgeonavigationhelper.h \
    qgeonavigationinstruction.h \
    qgeoroute.h \
    qgeoroutereply.h \
    qgeorouterequestoptions.h \
    qgeoroutesegment.h \
    qgeoroutingservice.h \
    qgeoserviceprovider.h

PRIVATE_HEADERS += \
    qgeoboundingbox_p.h \
    qgeocodereply_p.h \
    qgeocoderequestoptions_p.h \
    qgeocodingservice_p.h \
    qgeodistance_p.h \
    qgeomapobject_p.h \
    qgeomappingservice_p.h \
    qgeomapreply_p.h \
    qgeomaprequestoptions_p.h \
    qgeomapwidget_p.h \
    qgeonavigationhelper_p.h \
    qgeonavigationinstruction_p.h \
    qgeoroute_p.h \
    qgeoroutereply_p.h \
    qgeorouterequestoptions_p.h \
    qgeoroutingservice_p.h \
    qgeoroutesegment_p.h \
    qgeoserviceprovider_p.h

HEADERS = $$PUBLIC_HEADERS $$PRIVATE_HEADERS

SOURCES += \
    qgeoboundingbox.cpp \
    qgeocodereply.cpp \
    qgeocoderequestoptions.cpp \
    qgeocodingservice.cpp \
    qgeodistance.cpp \
    qgeomapobject.cpp \
    qgeomappingservice.cpp \
    qgeomapreply.cpp \
    qgeomaprequestoptions.cpp \
    qgeomapwidget.cpp \
    qgeonavigationhelper.cpp \
    qgeonavigationinstruction.cpp \
    qgeoroute.cpp \
    qgeoroutereply.cpp \
    qgeorouterequestoptions.cpp \
    qgeoroutesegment.cpp \
    qgeoroutingservice.cpp \
    qgeoserviceprovider.cpp

CONFIG += middleware
include(../../../features/deploy.pri)

