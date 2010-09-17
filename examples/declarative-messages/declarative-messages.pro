QT += declarative network
SOURCES += $$PWD/qmlmessages.cpp
include($$PWD/deployment.pri)

TARGET = qml_messages

symbian {
    TARGET.CAPABILITY = NetworkServices ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
