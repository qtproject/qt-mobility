include(deployment.pri)
include(declarative-sfw-notes.pri)

QT += declarative network script
TEMPLATE=app

SOURCES += qmlnotes.cpp

TARGET = qmlnotes

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
