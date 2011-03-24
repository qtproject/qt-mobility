include(../../mobility_examples.pri)

QT += declarative
SOURCES += device.cpp
TARGET = qml_device
TEMPLATE = app

win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=systeminfo
}

symbian {
    TARGET.CAPABILITY = ReadUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
RESOURCES += device.qrc

QMAKE_LFLAGS_DEBUG += "-Wl,-rpath,/home/user/qt/lib"
QMAKE_LFLAGS_RPATH += "-Wl,-rpath,/home/user/qt/lib"
QMAKE_LFLAGS_RELEASE += "-Wl,-rpath,/home/user/qt/lib"
