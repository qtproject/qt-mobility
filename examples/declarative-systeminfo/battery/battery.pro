include($$PWD/deployment.pri)
include($$PWD/../declarative-systeminfo.pri)

QT += declarative
SOURCES += $$PWD/qmlbattery.cpp
TARGET = qml_battery

symbian {
    TARGET.CAPABILITY = ReadUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
RESOURCES += declarative-battery.qrc
