QT += declarative network
SOURCES += $$PWD/qmlposter.cpp
include($$PWD/../declarative-connectivity.pri)

TARGET = qml_poster

symbian {
    TARGET.CAPABILITY = LocalServices
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

RESOURCES += \
    poster.qrc

OTHER_FILES += \
    poster.qml
