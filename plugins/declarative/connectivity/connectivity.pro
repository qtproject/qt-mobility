INCLUDEPATH += . \
               ../../../src/global \
               ../../../include \
               ../../../src/connectivity \
               ../../../src/connectivity/bluetooth \
               ../../../src/connectivity/nfc

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
    qdeclarativebluetoothservice_p.h \
    qdeclarativebluetoothsocket_p.h \
    qdeclarativenearfieldsocket_p.h \
    qdeclarativenearfield_p.h \
    qdeclarativendeffilter_p.h \
    qdeclarativendeftextrecord_p.h \
    qdeclarativendefurirecord_p.h \
    qdeclarativendefmimerecord_p.h \
    qdeclarativebluetoothimageprovider_p.h \
    qdeclarativebluetoothdiscoverymodel_p.h

SOURCES += plugin.cpp \ 
    qdeclarativebluetoothservice.cpp \
    qdeclarativebluetoothsocket.cpp \
    qdeclarativenearfieldsocket.cpp \
    qdeclarativenearfield.cpp \
    qdeclarativendeffilter.cpp \
    qdeclarativendeftextrecord.cpp \
    qdeclarativendefurirecord.cpp \
    qdeclarativendefmimerecord.cpp \
    qdeclarativebluetoothdiscoverymodel.cpp \
    qdeclarativebluetoothimageprovider.cpp

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
