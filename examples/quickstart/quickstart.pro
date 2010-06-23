TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += network

#! [0]
CONFIG += mobility
MOBILITY += systeminfo
#! [0]

# Input
SOURCES += main.cpp

#! [1]
symbian {
    TARGET.CAPABILITIES = LocalServices ReadUserData WriteUserData NetworkServices UserEnvironment Location ReadDeviceData
}
#! [1]
