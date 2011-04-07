include(../../mobility_examples.pri)

QT += declarative network
SOURCES += qmllocationflickr.cpp
TARGET = qml_location_flickr
TEMPLATE=app

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=location
}

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    flickr.qrc
