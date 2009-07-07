TEMPLATE = lib
CONFIG += plugin
TARGET = qwmp

DEFINES += QWMP_EVR

DESTDIR = $$OUT_PWD/../../lib
DLLDESTDIR = $$OUT_PWD/../../bin/mediaservice

LIBS += -L$$OUT_PWD/../../lib -lQtMedia -lstrmiids -lole32 -lOleaut32

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

HEADERS = \
    qevrwidget.h \
    qmfactivate.h \
    qwmpevents.h \
    qwmpglobal.h \
    qwmpmetadata.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h \
    qwmpplaylist.h \
    qwmpplaylistproxy.h \
    qwmpserviceprovider.h

SOURCES = \
    qevrwidget.cpp \
    qmfactivate.cpp \
    qwmpevents.cpp \
    qwmpglobal.cpp \
    qwmpmetadata.cpp \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp \
    qwmpplaylist.cpp \
    qwmpplaylistproxy.cpp \
    qwmpserviceprovider.cpp

