include(../../mobility_examples.pri)

QT += declarative network

SOURCES += \
    qmltennis.cpp \
    controller.cpp

HEADERS += \
    controller.h


TARGET = qml_tennis
TEMPLATE = app

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=connectivity
}

symbian {
    TARGET.CAPABILITY = LocalServices NetworkServices Location ReadUserData WriteUserData WriteDeviceData ReadDeviceData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    tennis.qrc

OTHER_FILES += \
    tennis.qml \
    sensor.qml \
    Handover.qml \
    Paddle.qml \
    Bumper.qml \
    Score.qml \
    ConnectButton.qml
