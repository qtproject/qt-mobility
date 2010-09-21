include(deployment.pri)
include(declarative-sfw-notes.pri)

QT += declarative network script
TEMPLATE = app

SOURCES += qmlnotes.cpp

TARGET = qmlnotes

qml.path = $$QT_MOBILITY_EXAMPLES/declarative-sfw-notes/
qml.files = content

symbian {
    qml.files += declarative-sfw-notes.qml
} maemo* {
    qml.files += maemo-sfw-notes.qmil
} else {
    qml.files += declarative-sfw-notes.qml
}

INSTALLS += qml

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
