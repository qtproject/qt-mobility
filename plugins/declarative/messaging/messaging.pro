INCLUDEPATH += ../../../src/messaging
INCLUDEPATH += ../../../src/global

TEMPLATE = lib
CONFIG += plugin
TARGET  = $$qtLibraryTarget(declarative_messaging)
TARGETPATH = QtMobility/messaging
PLUGIN_TYPE = declarative

include(../../../common.pri)

QT += declarative

SOURCES += \
    qdeclarativemessagefilter.cpp \
    qdeclarativemessagemodel.cpp \
    plugin.cpp

HEADERS += \
    qdeclarativemessagefilter.h \
    qdeclarativemessagemodel.h


CONFIG += mobility
MOBILITY += messaging

target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

# QMF libraries must be located at $QMF_LIBDIR
simulator|contains(qmf_enabled, yes) {
    mac {
        QMAKE_LFLAGS += -F$$(QMF_LIBDIR)
        LIBS += -framework qmfclient
    } else {
        maemo6:meego {
            CONFIG += link_pkgconfig
            PKGCONFIG += qmfclient
        } else {
            LIBS += -L$$(QMF_LIBDIR) -l$$qtLibraryTarget(qmfclient)
        }
    }
}

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x20031601
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_messaging$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }

