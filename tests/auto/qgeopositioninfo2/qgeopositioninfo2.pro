#-------------------------------------------------
#
# Project created by QtCreator 2010-11-10T10:08:22
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

QT += xml network svg

TARGET = tst_qgepositioninfo2
CONFIG   += console
CONFIG   -= app_bundle
CONFIG+=testcase
CONFIG += mobility
MOBILITY = location

TEMPLATE = app

INCLUDEPATH += ../../src/global \
               ../../src/bearer \
               ../../src/location \
               ../../src/location/maps

SOURCES += tst_qgeopositioninfo.cpp

HEADERS += \
    tst_qgeopositioninfo.h
