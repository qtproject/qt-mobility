######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = samplephonebook
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lQtContacts

include(../../common.pri)

# Input
SOURCES +=  contactdetailsform.cpp \
            maindialogform_240_320.cpp \
            maindialogform_640_480.cpp \
            phonebook.cpp \
            serialiser.cpp \
            finddialog.cpp \
            main.cpp
HEADERS   = contactdetailsform.h \
            maindialogform_240_320.h \
            maindialogform_640_480.h \
            phonebook.h \
            serialiser.h \
            finddialog.h

FORMS += contactdetails_240_320.ui \
         maindialog_240_320.ui \
         maindialog_640_480.ui 

symbian: {
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData \
                        ReadDeviceData \
                        WriteDeviceData \
                        AllFiles \
                        SwEvent
}
