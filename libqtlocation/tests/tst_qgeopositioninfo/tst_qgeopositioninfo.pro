target.path = /usr/lib/libqtlocation-tests
INSTALLS += target

TEMPLATE = app
TARGET = tst_qgeopositioninfo
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
SOURCES += tst_qgeopositioninfo.cpp

QMAKE_DISTCLEAN += -r .moc .obj





