TEMPLATE = app
TARGET = todotool
INCLUDEPATH += ../../src/serviceframework

include(../examples.pri)

QT += gui

# Input 
HEADERS += todotool.h
SOURCES += todotool.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += resource.qrc

win32 {
    FORMS += todotool.ui
}

unix: {
    linux-*: {
        FORMS += todotool.ui
    }

    mac: {
        FORMS += todotool.ui
    }
}

symbian {
    addFiles.sources = ../notesmanagerplugin/notesmanagerservice.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles

    TARGET.CAPABILITY = ALL -TCB
    FORMS += todotool.ui #change to todotool_s60.ui
}
