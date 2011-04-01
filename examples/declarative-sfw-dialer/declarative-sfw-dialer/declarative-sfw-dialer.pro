include(../../mobility_examples.pri)

INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework

QT += declarative network script
SOURCES += qmldialer.cpp
TARGET = qmldialer
TEMPLATE = app
RESOURCES += declarative-sfw-dialer.qrc

CONFIG += mobility
MOBILITY += serviceframework

symbian {
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
