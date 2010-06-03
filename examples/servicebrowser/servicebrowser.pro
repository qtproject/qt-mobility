TEMPLATE = app
TARGET = servicebrowser
INCLUDEPATH += ../../src/serviceframework

include(../examples.pri)

QT += gui

# Input 
HEADERS += servicebrowser.h
SOURCES += servicebrowser.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

symbian {
    addFiles.sources = ../filemanagerplugin/filemanagerservice.xml
    addFiles.sources += ../bluetoothtransferplugin/bluetoothtransferservice.xml
    addFiles.path = xmldata
    DEPLOYMENT += addFiles

    TARGET.CAPABILITY = ReadUserData WriteUserData
}

mac {
    addFiles.sources = ../filemanagerplugin/filemanagerservice.xml
    addFiles.sources += ../bluetoothtransferplugin/bluetoothtransferservice.xml
    addFiles.path = servicebrowser.app/Contents/MacOS/xmldata
    DEPLOYMENT += addFiles
}
