QT += declarative network

SOURCES += \
    qmltennis.cpp \
    controller.cpp

HEADERS += \
    controller.h

include($$PWD/../declarative-connectivity.pri)

TARGET = qml_tennis

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices Location ReadUserData WriteUserData WriteDeviceData ReadDeviceData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    tennis.qrc

OTHER_FILES += \
    sensor.qml \
    Handover.qml \
    Paddle.qml \
    Bumper.qml \
    Score.qml \
    ConnectButton.qml
