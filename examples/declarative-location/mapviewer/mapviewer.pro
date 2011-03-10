QT += declarative network
SOURCES += $$PWD/qmlmapviewer.cpp
include($$PWD/../declarative-location.pri)
RESOURCES=mapviewer.qrc

TARGET = qml_mapviewer

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
