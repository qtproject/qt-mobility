TEMPLATE = app
TARGET = publish-subscribe

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

HEADERS = providerdialog.h \
          consumerdialog.h

SOURCES = main.cpp \
          providerdialog.cpp \
          consumerdialog.cpp

FORMS = providerdialog.ui \
        consumerdialog.ui

include(../examples.pri)
qtAddLibrary(QtPublishSubscribe)
