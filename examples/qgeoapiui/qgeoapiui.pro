
QT += xml network

TARGET = QGeoApiUI
TEMPLATE = app thread

INCLUDEPATH += ../../src/global \
               ../../src/location

SOURCES += main.cpp\
        routepresenter.cpp \
        placepresenter.cpp \
        tabbeddialog.cpp \
        routetab.cpp \
        geocodingtab.cpp \
        revgeocodingtab.cpp
        #maptiletab.cpp

HEADERS  += routepresenter.h \
        placepresenter.h \
        tabbeddialog.h \
        routetab.h \
        geocodingtab.h \
        revgeocodingtab.h
        #maptiletab.h

include(../examples.pri)

CONFIG += mobility
MOBILITY = location

symbian: {
    INCLUDEPATH += ../../src/bearer
    MOBILITY += bearer
    TARGET.CAPABILITY = Location NetworkServices ReadUserData WriteUserData
}
