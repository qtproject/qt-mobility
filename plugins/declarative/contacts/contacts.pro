INCLUDEPATH += . \
               ../../../src/global \
               ../../../include \
               ../../../src/contacts \
               ../../../src/contacts/requests \
               ../../../src/contacts/details \
               ../../../src/contacts/filters \
               ../../../src/versit
DEPENDPATH += ../../../src/contacts

TARGET = $$qtLibraryTarget(declarative_contacts)
TARGETPATH = QtMobility/contacts

TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE = declarative

include(../../../common.pri)

QT += declarative


CONFIG += mobility
MOBILITY = contacts versit
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH


# Input
HEADERS += qmlcontactmodel.h \
           imageprovider.h \
           qmlcontact.h \
           qmlcontactdetail.h \
           qmlcontactdetailfield.h

SOURCES += plugin.cpp \
    qmlcontactmodel.cpp \
    imageprovider.cpp \
    qmlcontact.cpp \
    qmlcontactdetail.cpp \
    qmlcontactdetailfield.cpp

OTHER_FILES += qmldir
    
INSTALLS += target qmldir
