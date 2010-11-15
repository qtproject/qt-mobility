include(declarative-sfw-dialer.pri)
include(deployment.pri)

QT += declarative network script
SOURCES += qmldialer.cpp
TARGET = qmldialer
RESOURCES += declarative-sfw-dialer.qrc

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
