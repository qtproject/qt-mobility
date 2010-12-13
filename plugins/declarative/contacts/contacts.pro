INCLUDEPATH += . \
               ../../../src/global \
               ../../../include \
               ../../../src/contacts \
               ../../../src/contacts/requests \
               ../../../src/contacts/details \
               ../../../src/contacts/filters \
               ../../../src/versit
DEPENDPATH += ../../../src/contacts

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(declarative_contacts)
TARGETPATH = QtMobility/contacts

PLUGIN_TYPE = declarative

include(../../../common.pri)
include(details/details.pri)
include(filters/filters.pri)

QT += declarative script network


CONFIG += mobility
MOBILITY = contacts versit
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH


# Input
HEADERS += qdeclarativecontactmodel_p.h \
           qdeclarativecontact_p.h \
           qdeclarativecontactdetail_p.h \
           qdeclarativeopenmetaobject_p.h \
           qmetaobjectbuilder_p.h \
           qdeclarativecontactfilter_p.h \
           qdeclarativecontactmetaobject_p.h \
           qdeclarativecontactimageprovider_p.h \
           qdeclarativecontactsortorder_p.h \
           qdeclarativecontactfetchhint_p.h \
           qdeclarativecontactrelationship_p.h \
           qdeclarativecontactrelationshipmodel_p.h

SOURCES += plugin.cpp \
    qdeclarativecontactmodel.cpp \
    qdeclarativecontact.cpp \
    qdeclarativecontactdetail.cpp \
    qdeclarativeopenmetaobject.cpp \
    qmetaobjectbuilder.cpp \
    qdeclarativecontactfilter.cpp \
    qdeclarativecontactmetaobject.cpp \
    qdeclarativecontactimageprovider.cpp \
    qdeclarativecontactsortorder.cpp \
    qdeclarativecontactfetchhint.cpp \
    qdeclarativecontactrelationship.cpp \
    qdeclarativecontactrelationshipmodel.cpp

RESOURCES += contacts.qrc

INSTALLS += qmldir

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


