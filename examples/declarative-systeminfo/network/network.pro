include($$PWD/deployment.pri)
include($$PWD/../declarative-systeminfo.pri)

QT += declarative
SOURCES += $$PWD/qmlnetwork.cpp
TARGET = qml_networkinfo

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=systeminfo
}

symbian {
    TARGET.CAPABILITY = ReadUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
RESOURCES += declarative-network.qrc
