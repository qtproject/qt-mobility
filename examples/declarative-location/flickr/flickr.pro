include($$PWD/../declarative-location.pri)

QT += declarative network
SOURCES += $$PWD/qmllocationflickr.cpp
TARGET = qml_location_flickr
RESOURCES=flickr.qrc

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
