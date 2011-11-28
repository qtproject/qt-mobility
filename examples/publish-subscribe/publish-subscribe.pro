TEMPLATE = app
TARGET = publish-subscribe

INCLUDEPATH += ../../src/publishsubscribe
DEPENDPATH += ../../src/publishsubscribe

symbian {
    crmlFiles.sources = example.qcrml
    crmlFiles.path = /resource/qt/crml
    DEPLOYMENT += crmlFiles

    TARGET.UID3 = 0xE0000020
}

include(../mobility_examples.pri)

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

contains(MEEGO_EDITION,harmattan) {

    MEEGO_VERSION_MAJOR     = 1
    MEEGO_VERSION_MINOR     = 2
    MEEGO_VERSION_PATCH     = 0
    MEEGO_EDITION           = harmattan
    DEFINES += MEEGO_EDITION_HARMATTAN


    target.path = /opt/publish-subscribe/bin
    INSTALLS += target

    contextreg.files = com.qt.nokia.context
    contextreg.path = /usr/share/contextkit/providers
    INSTALLS += contextreg

}

OTHER_FILES += \
    com.qt.nokia.context



