target.path = /usr/lib/libqtlocation-tests
INSTALLS += target

TEMPLATE = app
TARGET = tst_qsatelliteinfo
CONFIG+=testcase
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .
QT *= testlib
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../../src
QMAKE_CXXFLAGS += -Wall
LIBS += -L../../src -lQtLocation

# Input 
SOURCES += tst_qsatelliteinfo.cpp

QMAKE_DISTCLEAN += -r .moc .obj

