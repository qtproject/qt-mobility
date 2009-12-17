TEMPLATE=app
TARGET=qcrmlgen
CONFIG += core gui

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

HEADERS = qcrmlgen.h \
          qcrmlparser_p.h

SOURCES=\
    main.cpp \
    qcrmlgen.cpp \
    qcrmlparser.cpp

include(../../common.pri)
CONFIG += mobility
MOBILITY = publishsubscribe
include(../../features/deploy.pri)
