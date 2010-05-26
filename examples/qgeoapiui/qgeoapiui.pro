QT += xml network

TARGET = QGeoApiUI
TEMPLATE = app thread

INCLUDEPATH += $$QT_MOBILITY_BUILD_TREE/include/QtmLocation
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/location

SOURCES += main.cpp\
        routepresenter.cpp \
        placepresenter.cpp \
        tabbedwindow.cpp \
        routetab.cpp \
        geocodingtab.cpp \
        revgeocodingtab.cpp \
        maptiletab.cpp \
        servicestab.cpp

HEADERS  += routepresenter.h \
        placepresenter.h \
        tabbedwindow.h \
        routetab.h \
        geocodingtab.h \
        revgeocodingtab.h \
        maptiletab.h \
        servicestab.h

include(../examples.pri)

CONFIG += mobility
MOBILITY = location

symbian: {
    MOBILITY += bearer
    INCLUDEPATH += $$QT_MOBILITY_BUILD_TREE/include/QtmBearer
    INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/src/bearer
    TARGET.CAPABILITY = Location NetworkServices ReadUserData WriteUserData
}
