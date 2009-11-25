TEMPLATE = app
TARGET = publish-subscribe

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

symbian {
    crmlFiles.sources = example.qcrml
    crmlFiles.path = c:/resource/qt/crml
    DEPLOYMENT += crmlFiles

    TARGET.UID3 = 0x2002AC79
}


HEADERS = publisherdialog.h \
          subscriberdialog.h

SOURCES = main.cpp \
          publisherdialog.cpp \
          subscriberdialog.cpp

FORMS = publisherdialog.ui \
        subscriberdialog.ui

include(../examples.pri)
qtAddLibrary(QtPublishSubscribe)
