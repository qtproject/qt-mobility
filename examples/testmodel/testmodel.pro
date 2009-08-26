######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = testmodel
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lQtContacts

include(../../common.pri)

# Input
SOURCES += main.cpp \
           testmodelui.cpp \
           qcontactlistmodel.cpp

HEADERS += testmodelui.h \
           qcontactlistmodel.h \
           qcontactlistmodel_p.h

