TARGET = PublishSubscribeEx
TEMPLATE = app

QT += core \
      gui \
      network

include(../../common.pri)
#not really a test case but deployment happens same way
CONFIG += testcase
DEPENDPATH += .
INCLUDEPATH += . \
               ../../src/publishsubscribe

SOURCES += main.cpp\
        publishsubscribemainwindow.cpp

HEADERS  += publishsubscribemainwindow.h

FORMS    += publishsubscribemainwindow.ui

symbian {
    crml.sources = resources.qcrml profile.qcrml
    crml.path = /resource/qt/crml
    DEPLOYMENT += crml
    TARGET.CAPABILITY = ReadUserData
    TARGET.UID3 = 0x2002ac7c
}

CONFIG += mobility
MOBILITY = publishsubscribe
