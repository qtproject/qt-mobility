######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = samplephonebook
DEPENDPATH += .
INCLUDEPATH += . \
               ../../contacts\
               ../../contacts/filters \
               ../../contacts/requests \
               ../../contacts/details 

INCLUDEPATH += ../testmodel
VPATH += ../testmodel


qtAddLibrary(QtContacts)

# Input
SOURCES =  contactdetailsform.cpp \
            maindialogform_240_320.cpp \
            maindialogform_640_480.cpp \
            phonebook.cpp \
            serialiser.cpp \
            groupeditdialog.cpp \ 
            groupdetailsdialog.cpp \
            main.cpp \
            finddialog.cpp

HEADERS   = contactdetailsform.h \
            maindialogform_240_320.h \
            maindialogform_640_480.h \
            phonebook.h \
            serialiser.h \
            finddialog.h \
            ../incomingcalls/filterdialog.h \
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
include(../examples.pri)
qtAddLibrary(QtContacts)
