TARGET = sfwecho_service
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

include(../../examples.pri)

QT = core
TEMPLATE = app

CONFIG += mobility no_icon
MOBILITY = serviceframework

HEADERS += 
SOURCES += main.cpp

wince*|symbian*: {
    addFiles.sources = sfwechoservice.xml
    addFiles.path = xmldata
    addFiles2.sources = sfwechoservice.xml
    addFiles2.path = /private/2002AC7F/import/
    DEPLOYMENT += addFiles addFiles2
}
wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = sfwechoservice.xml
INSTALLS += xml

