TARGET = sfwecho_service
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

QT = core
TEMPLATE=app

include(../../examples.pri)

CONFIG += mobility
MOBILITY = serviceframework

SOURCES += main.cpp
HEADERS += 

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

wince*|symbian*: {
    addFiles.sources = testdata/*
    addFiles.path = xmldata
    DEPLOYMENT += addFiles
}
wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = testdata/sfwechoservice.xml
INSTALLS += xml

