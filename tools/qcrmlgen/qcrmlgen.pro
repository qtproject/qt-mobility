TEMPLATE=app
TARGET=qcrmlgen
CONFIG += core gui

INCLUDEPATH += ../../context
DEPENDPATH += ../../context

#enable_readline {
#    MODULES*=readline
#    DEFINES+=USE_READLINE
#}
HEADERS = qcrmlgen.h \
          qcrmlparser_p.h

SOURCES=\
    main.cpp \
    qcrmlgen.cpp \
    qcrmlparser.cpp

include(../../common.pri)
LIBS += -lQtPublishSubscribe
