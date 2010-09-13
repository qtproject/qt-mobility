QT += declarative network
SOURCES += $$PWD/qmllandmarkmap.cpp
include($$PWD/deployment.pri)
include($$PWD/../declarative-location.pri)

TARGET = qml_landmarkmap

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
