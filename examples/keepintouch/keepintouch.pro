TEMPLATE = app
TARGET = keepintouch

QT += gui

include(../examples.pri)

qtAddLibrary(QtMessaging)
INCLUDEPATH += ../../src/messaging 

qtAddLibrary(QtContacts)
INCLUDEPATH += ../../src/contacts ../../src/contacts/details ../../src/contacts/filters ../../src/contacts/requests

symbian|win32 {
} else {
# Temporarily link against local qtopiamail lib (should be part of the platform)
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail
}

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

