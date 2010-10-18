TEMPLATE = app
TARGET = keepintouch

QT += gui

include(../examples.pri)

CONFIG += mobility
MOBILITY = messaging contacts

INCLUDEPATH += ../../src/messaging 
INCLUDEPATH += ../../src/contacts ../../src/contacts/details ../../src/contacts/filters ../../src/contacts/requests

HEADERS += \
    addressfinder.h

SOURCES += \
    addressfinder.cpp\
    main.cpp
    
symbian:TARGET.CAPABILITY = NetworkServices \
        LocalServices \
        ReadUserData \
        WriteUserData \
        UserEnvironment \
        ReadDeviceData \
        WriteDeviceData

