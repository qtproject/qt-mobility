QT += declarative network
SOURCES += $$PWD/qmlmapviewer.cpp
include($$PWD/deployment.pri)
include($$PWD/../declarative-location.pri)

TARGET = qml_mapviewer

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
