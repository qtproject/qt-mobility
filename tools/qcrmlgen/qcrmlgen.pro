TEMPLATE=app
TARGET=qcrmlgen
CONFIG += core gui

INCLUDEPATH += ../../src/publishsubscribe
INCLUDEPATH += ../../src/publishsubscribe/psmapperserver_symbian

HEADERS = qcrmlgen.h \
          ../../src/publishsubscribe/psmapperserver_symbian/qcrmlparser_p.h

SOURCES=\
    main.cpp \
    qcrmlgen.cpp \
    ../../src/publishsubscribe/psmapperserver_symbian/qcrmlparser.cpp

include(../../common.pri)
CONFIG += mobility
MOBILITY = publishsubscribe
include(../../features/deploy.pri)
