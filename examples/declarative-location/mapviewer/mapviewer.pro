include (../../mobility_examples.pri)

QT += declarative network
SOURCES += qmlmapviewer.cpp

TARGET = qml_mapviewer
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
    mapviewer.qrc
