target.path = /usr/lib/libqtlocation-tests
INSTALLS += target

TEMPLATE = app
TARGET = tst_qgeocoordinate
CONFIG+=testcase
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEPENDPATH += .

QT *= testlib
CONFIG += console
CONFIG -= app_bundle
# OBJECTS_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET
#  DESTDIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/bin
#  MOC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/moc
#  RCC_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/rcc
#  UI_DIR = $$OUTPUT_DIR/build/tests/$$SUBDIRPART/$$TARGET/ui
#  LIBS += -L$$OUTPUT_DIR/build/$$SUBDIRPART/bin  #link against library that we test

CONFIG += mobility
MOBILITY = location


INCLUDEPATH += ../../src
QMAKE_CXXFLAGS += -Wall
LIBS += -L../../src -lQtLocation

# Input 
# HEADERS += ../qlocationtestutils_p.h
SOURCES += tst_qgeocoordinate.cpp 
#\           ../qlocationtestutils.cpp

QMAKE_DISTCLEAN += -r .moc .obj


