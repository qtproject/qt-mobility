######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = samplephonebook
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../testmodel
VPATH += ../testmodel
LIBS += -lQtContacts

include(../../common.pri)

# Input
SOURCES +=  contactdetailsform.cpp \
            maindialogform_240_320.cpp \
            maindialogform_640_480.cpp \
            phonebook.cpp \
            serialiser.cpp \
            finddialog.cpp \
            filterdialog.cpp \ 
            groupeditdialog.cpp \ 
            groupdetailsdialog.cpp \
            main.cpp
HEADERS   = contactdetailsform.h \
            maindialogform_240_320.h \
            maindialogform_640_480.h \
            phonebook.h \
            serialiser.h \
            finddialog.h \
            filterdialog.h \
            groupeditdialog.h \
            groupdetailsdialog.h

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
