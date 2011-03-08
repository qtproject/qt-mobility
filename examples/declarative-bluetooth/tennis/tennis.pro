QT += declarative network
SOURCES += $$PWD/qmltennis.cpp
include($$PWD/../declarative-bluetooth.pri)
include($$PWD/deployment.pri)

TARGET = qml_tennis

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices Location ReadUserData WriteUserData WriteDeviceData ReadDeviceData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    tennis.qrc

OTHER_FILES += \
    sensor.qml
