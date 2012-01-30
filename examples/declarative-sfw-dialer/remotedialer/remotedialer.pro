TARGET = dialer_service
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

include(../../mobility_examples.pri)

QT = core
TEMPLATE = app

CONFIG += mobility no_icon
MOBILITY = serviceframework

contains(QT_CONFIG, declarative):DEFINES += DECLARATIVE

HEADERS += remotedialer.h
SOURCES += remotedialer.cpp \
           main.cpp     

wince*|symbian*: {
    addFiles.sources = remotedialerservice.xml
    addFiles.path = xmldata
    addFiles2.sources = remotedialerservice.xml
    addFiles2.path = /private/2002AC7F/import/
    DEPLOYMENT += addFiles addFiles2
}
wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = remotedialerservice.xml
INSTALLS += xml

contains(MEEGO_EDITION,harmattan) {
    xml.path = /opt/declarative-sfw-dialer/bin/xmldata
    xml.files = remotedialerservice.xml
    INSTALLS += xml

    remotedialer_conf.files = remotedialer.conf
    remotedialer_conf.path = /etc/dbus-1/session.d/

    remotedialer_service.files = com.nokia.qt.examples.dialer.service
    remotedialer_service.path = /usr/share/dbus-1/services/

    target.path = /opt/remotedialer/bin

    INSTALLS += target remotedialer_conf remotedialer_service
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog


