TEMPLATE = app
TARGET = publish-subscribe

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
