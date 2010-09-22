DEPENDPATH += .
INCLUDEPATH += . \
    ../../../include \
    ../../../src/organizer \
    ../../../src/organizer/requests \
    ../../../src/organizer/details \
    ../../../src/organizer/filters \
    ../../../src/versit

TARGET = $$qtLibraryTarget(declarative_organizer)
TARGETPATH = QtMobility/organizer

TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE = declarative

include(../../../common.pri)

QT += declarative
QT += script network


CONFIG += mobility
MOBILITY = organizer

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH



# Input
HEADERS += qdeclarativeorganizermodel_p.h \
           qdeclarativeorganizeritem_p.h \
           qdeclarativeorganizeritemdetail_p.h \
           qdeclarativeorganizer_p.h \
           qmetaobjectbuilder_p.h \
           qdeclarativeopenmetaobject_p.h \
           qdeclarativeorganizeritemfilter_p.h \
           qdeclarativeorganizeritemmetaobject_p.h

SOURCES += plugin.cpp \
           qmllorganizermodel.cpp \
           qdeclarativeorganizeritem.cpp \
           qdeclarativeorganizeritemdetail.cpp \
           qdeclarativeorganizer.cpp \
           qmetaobjectbuilder.cpp \
           qdeclarativeopenmetaobject.cpp \
           qdeclarativeorganizeritemmetaobject.cpp


qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

OTHER_FILES += qmldir

INSTALLS += target qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x20021326
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_organizer$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
