TARGET = sfwecho_service
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

include(../../examples.pri)

QT = core
TEMPLATE=app

CONFIG += mobility
MOBILITY = serviceframework

HEADERS += 
SOURCES += main.cpp

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

