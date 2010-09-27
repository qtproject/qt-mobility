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
!mac:TARGET = qml_camera
else:TARGET = QmlCamera

RESOURCES += declarative-camera.qrc

symbian {
    load(data_caging_paths)
    TARGET.CAPABILITY = UserEnvironment NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

