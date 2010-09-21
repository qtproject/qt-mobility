include(deployment.pri)
include(declarative-sfw-dialer.pri)

QT += declarative network script
TEMPLATE=app

SOURCES += qmldialer.cpp

TARGET = qmldialer

qml.path = $$QT_MOBILITY_EXAMPLES/declarative-sfw-dialer/
qml.files = content

symbian {
    qml.files += declarative-sfw-dialer.qml
} else {
    qml.files += declarative-sfw-dialer.qml
}

INSTALLS += qml

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
