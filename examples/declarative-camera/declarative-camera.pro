include(../../staticconfig.pri)
include (../examples.pri)

TEMPLATE=app

QT += declarative network

!maemo* {
    contains(QT_CONFIG, opengl) {
        QT += opengl
    }
}

SOURCES += $$PWD/qmlcamera.cpp
TARGET = qml_camera

RESOURCES += declarative-camera.qrc

symbian {
    load(data_caging_paths)
    #TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

