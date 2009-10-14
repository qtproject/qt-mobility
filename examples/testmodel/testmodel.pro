######################################################################
#
# Simple example of how to use the contacts API
#
######################################################################

TEMPLATE = app
TARGET = testmodel
DEPENDPATH += .
INCLUDEPATH += .

include(../../common.pri)

qtAddLibraryFixMe(QtContacts)

# Input
SOURCES += main.cpp \
           testmodelui.cpp \
           filterdialog.cpp \
           qcontactlistmodel.cpp

HEADERS += testmodelui.h \
           filterdialog.h \
           qcontactlistmodel.h \
           qcontactlistmodel_p.h

