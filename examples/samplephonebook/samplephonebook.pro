######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = samplephonebook
DEPENDPATH += .
INCLUDEPATH += . ../../contacts ../../contacts/$${CONTACTS_BACKEND} ../../util
LIBS += -lQtContacts

include(../../common.pri)

# Input
SOURCES +=  phonebook.cpp \
            serialiser.cpp \
            finddialog.cpp \
            main.cpp
HEADERS   = phonebook.h \
            serialiser.h \
            finddialog.h
