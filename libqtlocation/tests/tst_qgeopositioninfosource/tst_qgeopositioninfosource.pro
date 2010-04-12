######################################################################
# Unit test project for class GeoPoint 
######################################################################

target.path = /usr/lib/libqtlocation-tests
INSTALLS += target
TEMPLATE = app
TARGET = tst_qgeopositioninfosource
DEPENDPATH += .
MOC_DIR = .moc
OBJECTS_DIR = .obj
INCLUDEPATH += .
INCLUDEPATH += ../../src
QMAKE_CXXFLAGS += -g -Wall
QT -= gui
# QT += network

LIBS += -L../../src -lQtLocation

# Input
HEADERS += tst_qgeopositioninfosource.h
SOURCES += tst_qgeopositioninfosource.cpp

QMAKE_DISTCLEAN += -r .moc .obj
