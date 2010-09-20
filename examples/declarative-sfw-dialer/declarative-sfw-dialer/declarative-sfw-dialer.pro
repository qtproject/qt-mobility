include(deployment.pri)
include(declarative-sfw-dialer.pri)

QT += declarative network script
TEMPLATE=app

SOURCES += qmldialer.cpp

TARGET = qmldialer

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
