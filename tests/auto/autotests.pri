QT *= testlib
CONFIG += testcase

CONFIG(debug, debug|release) {
    SUBDIRPART = Debug
} else {
    SUBDIRPART = Release
}

OBJECTS_DIR = ../../$$SUBDIRPART/$$TARGET
DESTDIR = ../../$$SUBDIRPART/bin
MOC_DIR = ../../$$SUBDIRPART/$$TARGET/moc
RCC_DIR = ../../$$SUBDIRPART/$$TARGET/rcc
UI_DIR = ../../$$SUBDIRPART/$$TARGET/ui
