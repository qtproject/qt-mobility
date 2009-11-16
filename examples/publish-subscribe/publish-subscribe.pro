TEMPLATE = app
TARGET = publish-subscribe

symbian {
    crmlFiles.sources = example.qcrml
    crmlFiles.path = c:/resource/qt/crml
    DEPLOYMENT += crmlFiles

    TARGET.UID3 = 0x2002AC79
}

INCLUDEPATH += ../../context
DEPENDPATH += ../../context

HEADERS = providerdialog.h \
          consumerdialog.h

SOURCES = main.cpp \
          providerdialog.cpp \
          consumerdialog.cpp

FORMS = providerdialog.ui \
        consumerdialog.ui

include(../examples.pri)
qtAddLibrary(QtPublishSubscribe)
