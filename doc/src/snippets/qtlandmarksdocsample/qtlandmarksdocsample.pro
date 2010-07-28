TEMPLATE = app
TARGET = qtlandmarksdocsample

include(../../../../features/basic_examples_setup.pri)
INCLUDEPATH += ../../../../src/location

CONFIG += mobility
MOBILITY += location

SOURCES += qtlandmarksdocsample.cpp
HEADERS += requestexample.h
