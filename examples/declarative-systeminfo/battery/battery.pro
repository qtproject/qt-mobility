include(../../mobility_examples.pri)

QT += declarative
SOURCES += qmlbattery.cpp
TARGET = qml_battery
TEMPLATE = app

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
