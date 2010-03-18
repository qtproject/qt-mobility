
QT += xml network

TARGET = QGeoApiUI
TEMPLATE = app thread

INCLUDEPATH += ../../src/global \
                ../../src/location

SOURCES += main.cpp\
        mainwindow.cpp \
        routepresenter.cpp \
        placepresenter.cpp
HEADERS  += mainwindow.h \
        routepresenter.h \
        placepresenter.h

FORMS    += mainwindow.ui
include(../examples.pri)

CONFIG += mobility
MOBILITY = location

symbian: {
    MOBILITY += bearer
    TARGET.CAPABILITY = Location NetworkServices ReadUserData WriteUserData
}
