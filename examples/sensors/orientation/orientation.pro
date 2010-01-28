TEMPLATE=app
TARGET=orientation
include(../../examples.pri)
QT+=declarative
CONFIG+=mobility
MOBILITY+=sensors
INCLUDEPATH += ../../../src/sensors
SOURCES=main.cpp
RESOURCES = orientation.qrc
OTHER_FILES += orientation.qml

STRICT=$$(STRICT)
equals(STRICT,1) {
    QMAKE_CXXFLAGS+=-Werror
    QMAKE_LFLAGS+=-Wl,-no-undefined
}

