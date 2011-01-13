INCLUDEPATH += . \
               ../../../src/global \
               ../../../include \
               ../../../src/connectivity \
               ../../../src/connectivity/bluetooth

DEPENDPATH += ../../../src/connectivity

TARGET = $$qtLibraryTarget(declarative_connectivity)
TARGETPATH = QtMobility/connectivity

TEMPLATE = lib
CONFIG += plugin
PLUGIN_TYPE = declarative

include(../../../common.pri)

# support headers/sources for dynamic properties
#include(../common/dynamicproperties.pri)

QT += declarative script network

CONFIG += mobility
MOBILITY = connectivity
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH


# Input
HEADERS += \
    qdeclarativebtdiscoverymodel_p.h \
    qdeclarativebtimageprovider_p.h \
    qdeclarativebluetoothservice_p.h \
    qdeclarativebluetoothsocket_p.h

SOURCES += plugin.cpp \ 
    qdeclarativebtdiscoverymodel.cpp \
    qdeclarativebtimageprovider.cpp \
    qdeclarativebluetoothservice.cpp \
    qdeclarativebluetoothsocket.cpp

RESOURCES += connectivity.qrc

INSTALLS += qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
#    TARGET.UID3 = 0x20021325
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_connectivity$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
