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


HEADERS = providerdialog.h \
          consumerdialog.h

SOURCES = main.cpp \
          providerdialog.cpp \
          consumerdialog.cpp

FORMS = providerdialog.ui \
        consumerdialog.ui

include(../examples.pri)
qtAddLibrary(QtPublishSubscribe)
