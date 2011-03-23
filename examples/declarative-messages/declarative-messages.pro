include(../mobility_examples.pri)

QT += declarative network
TEMPLATE=app
SOURCES += qmlmessages.cpp
RESOURCES = declarative-messages.qrc

TARGET = qml_messages
win32 {
    #required by Qt SDK to resolve Mobility libraries
    CONFIG+=mobility
    MOBILITY+=messaging
}


symbian {
    TARGET.CAPABILITY = NetworkServices ReadUserData WriteUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}
