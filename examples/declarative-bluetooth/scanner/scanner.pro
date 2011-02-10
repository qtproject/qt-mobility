QT += declarative network
SOURCES += $$PWD/qmlscanner.cpp
include($$PWD/../declarative-bluetooth.pri)
include($$PWD/deployment.pri)

TARGET = qml_scanner

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    scanner.qrc
