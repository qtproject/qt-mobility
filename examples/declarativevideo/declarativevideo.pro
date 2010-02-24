TEMPLATE = app
TARGET = declarativevideo

QT += declarative

INCLUDEPATH+=../../src/multimedia
include (../examples.pri)

qtAddLibrary(QtMedia)

SOURCES = main.cpp

RESOURCES = \
        declarativevideo.qrc

OTHER_FILES = \
        player.qml
