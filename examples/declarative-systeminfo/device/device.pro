include($$PWD/deployment.pri)
include($$PWD/../declarative-systeminfo.pri)

QT += declarative
SOURCES += $$PWD/device.cpp
TARGET = qml_device

symbian {
    TARGET.CAPABILITY = ReadUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
RESOURCES += device.qrc

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,/home/user/qt/lib"
QMAKE_LFLAGS_RPATH += "-Wl,-rpath,/home/user/qt/lib"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,/home/user/qt/lib"
