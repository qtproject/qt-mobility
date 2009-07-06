TEMPLATE = lib
CONFIG += plugin
TARGET = qwmp

DEFINES += BUILD_WMP_LIB

DESTDIR = $$OUT_PWD/../../lib
DLLDESTDIR = $$OUT_PWD/../../bin/mediaservice

LIBS += -L$$OUT_PWD/../../lib -lQtMedia -lstrmiids -lole32 -lOleaut32

contains (DEFINES, QWMP_EVR) : LIBS += -levr

INCLUDEPATH += $$PWD/../../multimedia $$PWD/../../multimedia/endpoints

HEADERS = \
    qevrwidget.h \
    qmfactivate.h \
    qwmpevents.h \
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
    qwmpmetadata.cpp \
    qwmpplayercontrol.cpp \
    qwmpplayerservice.cpp \
    qwmpplaylist.cpp \
    qwmpplaylistproxy.cpp \
    qwmpserviceprovider.cpp

