TARGET = dialer_service
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

include(../../examples.pri)

QT = core
TEMPLATE = app

CONFIG += mobility no_icon
MOBILITY = serviceframework

contains(QT_CONFIG, declarative):DEFINES += DECLARATIVE

HEADERS += remotedialer.h
SOURCES += remotedialer.cpp \
           main.cpp     

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

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
