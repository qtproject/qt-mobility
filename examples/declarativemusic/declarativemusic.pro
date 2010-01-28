TEMPLATE = app
TARGET = declarativemusic

QT += declarative

INCLUDEPATH+=../../src/multimedia
include (../examples.pri)

qtAddLibrary(QtMedia)

SOURCES = main.cpp

RESOURCES = \
        declarativemusic.qrc

OTHER_FILES = \
        player.qml
