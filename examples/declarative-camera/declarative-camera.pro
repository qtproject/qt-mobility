include(../../staticconfig.pri)
include (../examples.pri)

TEMPLATE=app

QT += declarative network

!maemo* {
    QT += opengl
}

SOURCES += $$PWD/qmlcamera.cpp
TARGET = qml_camera

RESOURCES += declarative-camera.qrc

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

