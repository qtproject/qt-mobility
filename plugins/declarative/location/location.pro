TARGET  = $$qtLibraryTarget(declarative_location)
TEMPLATE = lib
CONFIG += qt plugin mobility
MOBILITY += location
PLUGIN_TYPE = declarative
QT += declarative

include(../../../common.pri)

# On some platforms, build both versions because debug and release
# versions are incompatible
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

# If for some reason the version information is empty, default to 4.7.0
# which is the earliest version with declarative support
isEmpty(QT_MAJOR_VERSION) {
   VERSION=4.7.0
} else {
   VERSION=$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}
}

# Where to search for the #include'd files
INCLUDEPATH += ../../../src/location
INCLUDEPATH += ../../../src/global

# The header and source files of this project. Note that qmake generates dependency information
# and automatically adds necessary 'moc' files as it sees Q_OBJECT macros' in header files
HEADERS += qdeclarativeposition.h \
           qdeclarativepositionsource.h
SOURCES += qdeclarativeposition.cpp \
           location.cpp \
           qdeclarativepositionsource.cpp

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)
TARGETPATH = QtMobility/location
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH
qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir 

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    # However, really needs only Location capability so feel free to reduce
    # depending on your signing capabilities.
    TARGET.CAPABILITY = ALL-TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Symbian packaging rules (e.g. vendorinfo, default icon)
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    # TODO acquire real UID, this is borrowed
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x2002131E
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_location$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
