QT += declarative network
SOURCES += $$PWD/qmltennis.cpp
include($$PWD/../declarative-bluetooth.pri)
include($$PWD/deployment.pri)

TARGET = qml_tennis

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    tennis.qrc

OTHER_FILES += \
    sensor.qml
