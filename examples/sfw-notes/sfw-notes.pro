TEMPLATE = app
TARGET = sfw-notes
INCLUDEPATH += ../../src/serviceframework

include(../examples.pri)

QT += gui

# Input 
HEADERS += sfwnotes.h
SOURCES += sfwnotes.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += sfwnotes.qrc

win32 {
    FORMS += sfwnotes.ui
}

unix: {
    linux-*: {
        FORMS += sfwnotes.ui
    }

    mac: {
        FORMS += sfwnotes.ui
    }
}

mac:CONFIG -= app_bundle

symbian {
    #addFiles.sources = ../../notesmanagerplugin/notesmanagerservice.xml
    #addFiles.path = xmldata
    #DEPLOYMENT += addFiles

    #TARGET.CAPABILITY = ALL -TCB
    #FORMS += sfwnotes.ui #change to sfwnotes_small.ui
}
