INCLUDEPATH += ../../../src/serviceframework
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_serviceframework)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/serviceframework
PLUGIN_TYPE = declarative
include(../../../common.pri)

QT += declarative

HEADERS += qdeclarativeservice.h

SOURCES += qdeclarativeservice.cpp \
           serviceframework.cpp 

CONFIG += mobility
MOBILITY += serviceframework

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

symbian {
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x20021323
    load(armcc_warnings)
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_serviceframework$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
}
