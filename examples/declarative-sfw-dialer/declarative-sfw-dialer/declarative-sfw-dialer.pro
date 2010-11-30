include(declarative-sfw-dialer.pri)
include(deployment.pri)

INCLUDEPATH += ../../src/serviceframework
DEPENDPATH += ../../src/serviceframework

QT += declarative network script
SOURCES += qmldialer.cpp
TARGET = qmldialer
RESOURCES += declarative-sfw-dialer.qrc

CONFIG += mobility
MOBILITY += serviceframework

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
