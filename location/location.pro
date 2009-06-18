TEMPLATE = lib
TARGET = QtLocation

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

include(../location.pri)

CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
} else {
    SUBDIRPART = Release
}

OBJECTS_DIR = ../$$SUBDIRPART/$$TARGET
DESTDIR = ../$$SUBDIRPART/bin
MOC_DIR = ../$$SUBDIRPART/$$TARGET/moc
RCC_DIR = ../$$SUBDIRPART/$$TARGET/rcc
UI_DIR = ../$$SUBDIRPART/$$TARGET/ui
