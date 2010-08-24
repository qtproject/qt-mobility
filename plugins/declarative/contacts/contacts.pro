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

QT += declarative script network


CONFIG += mobility
MOBILITY = contacts versit

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH


# Input
HEADERS += qmlcontactmodel.h \
           qmlcontact.h \
           qmlcontactdetail.h \
           qmlcontactdetailfield.h

SOURCES += plugin.cpp \
    qmlcontactmodel.cpp \
    qmlcontact.cpp \
    qmlcontactdetail.cpp \
    qmlcontactdetailfield.cpp

# Qt 4.7.0b2 and 4.7.0rc1 have a source break for declarativeimageprovider, so don't
# compile them for now.
# HEADERS += imageprovider.h
# SOURCES += imageprovider.cpp

INSTALLS += target qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x20021325
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_contacts$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
