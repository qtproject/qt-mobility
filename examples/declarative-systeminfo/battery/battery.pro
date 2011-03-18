include($$PWD/deployment.pri)
include($$PWD/../declarative-systeminfo.pri)

QT += declarative
SOURCES += $$PWD/qmlbattery.cpp
TARGET = qml_battery

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=systeminfo
}


symbian {
    TARGET.CAPABILITY = ReadUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
RESOURCES += declarative-battery.qrc
