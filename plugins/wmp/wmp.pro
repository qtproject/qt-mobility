TEMPLATE = lib
CONFIG += plugin
TARGET = qwmp

DEFINES += QWMP_EVR

DESTDIR = $$OUT_PWD/../../lib
DLLDESTDIR = $$OUT_PWD/../../bin/mediaservice

LIBS += -L$$OUT_PWD/../../lib -lQtMedia -lstrmiids -lole32 -lOleaut32 -luser32

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

HEADERS = \
    qmfactivate.h \
    qwmpevents.h \
    qwmpglobal.h \
    qwmpmetadata.h \
    qwmpplaceholderwidget.h \
    qwmpplayercontrol.h \
    qwmpplayerservice.h \
    qwmpplaylist.h \
    qwmpserviceprovider.h

SOURCES = \
    qmfactivate.cpp \
    qwmpevents.cpp \
    qwmpglobal.cpp \
    qwmpmetadata.cpp \
    qwmpplaceholderwidget.cpp \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp \
    qwmpplaylist.cpp \
    qwmpserviceprovider.cpp

