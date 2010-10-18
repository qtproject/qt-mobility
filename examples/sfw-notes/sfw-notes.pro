TEMPLATE = app
TARGET = sfw-notes
INCLUDEPATH += ../../src/serviceframework

include(../examples.pri)

QT += gui
CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += sfwnotes.qrc
HEADERS += sfwnotes.h
SOURCES += sfwnotes.cpp \
           main.cpp

symbian|maemo*|wince* {
    FORMS += sfwnotes_mobile.ui
}
else {
    FORMS += sfwnotes.ui
}

mac:CONFIG -= app_bundle

symbian {
    addFiles.sources = ../notesmanagerplugin/notesmanagerservice.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles

    TARGET.CAPABILITY = ALL -TCB
}
