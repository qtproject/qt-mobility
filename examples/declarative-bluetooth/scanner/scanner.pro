QT += declarative network
SOURCES += $$PWD/qmlscanner.cpp
include($$PWD/../declarative-bluetooth.pri)
include($$PWD/deployment.pri)

TARGET = qml_scanner

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices Location ReadUserData WriteUserData ReadDeviceData WriteDeviceData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    scanner.qrc
