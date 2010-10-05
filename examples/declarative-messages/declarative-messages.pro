QT += declarative network
SOURCES += $$PWD/qmlmessages.cpp
RESOURCES = declarative-messages.qrc

TARGET = qml_messages

symbian {
    TARGET.CAPABILITY = NetworkServices ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
