TEMPLATE = app
TARGET = battery-publisher
INCLUDEPATH += ../../../src/publishsubscribe
DEPENDPATH += ../../../src/publishsubscribe

HEADERS = batterypublisher.h

SOURCES = main.cpp \
          batterypublisher.cpp

FORMS = batterypublisher.ui

include(../../mobility_examples.pri)
CONFIG += mobility
MOBILITY = publishsubscribe

symbian: {
    TARGET.UID3 = 0xE0000010
    crmlFiles.sources = power.battery.qcrml
    crmlFiles.path = /resource/qt/crml
    DEPLOYMENT += crmlFiles
}

contains(MEEGO_EDITION,harmattan) {

    target.path = /opt/battery-publisher/bin
    INSTALLS += target

    contextreg.files = com.qt.powerbattery.context
    contextreg.path = /usr/share/contextkit/providers
    INSTALLS += contextreg

}

OTHER_FILES += \
    com.qt.powerbattery.context








