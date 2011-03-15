TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
QT+=core

CONFIG += link_pkgconfig
PKGCONFIG += QtSparql QtSparqlTrackerExtensions
