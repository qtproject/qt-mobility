TEMPLATE = app
TARGET = publish-subscribe

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

symbian {
    crmlFiles.sources = example.qcrml
    crmlFiles.path = /resource/qt/crml
    DEPLOYMENT += crmlFiles

    #This is Symbian Signed UID3. Needs to match with uidValue in example.qcrml.
    TARGET.UID3 = 0x2002AC79
}

include(../examples.pri)

HEADERS = publisherdialog.h \
          subscriberdialog.h

SOURCES = main.cpp \
          publisherdialog.cpp \
          subscriberdialog.cpp

maemo5|maemo6 {
    FORMS = publisherdialog_hor.ui \
            subscriberdialog_hor.ui
} else {
    FORMS = publisherdialog.ui \
            subscriberdialog.ui
}

CONFIG += mobility
MOBILITY = publishsubscribe
