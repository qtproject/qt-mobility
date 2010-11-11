TARGET  = $$qtLibraryTarget(declarative_location)
TEMPLATE = lib
CONFIG += qt plugin mobility
MOBILITY += location
PLUGIN_TYPE = declarative
QT += declarative network

include(../../../common.pri)

# On some platforms, build both versions because debug and release
# versions are incompatible
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release

# Where to search for the #include'd files
INCLUDEPATH += ../../../src/global
INCLUDEPATH += ../../../src/location
INCLUDEPATH += ../../../src/location/landmarks
INCLUDEPATH += ../../../src/location/maps
INCLUDEPATH += .

# The header and source files of this project. Note that qmake generates dependency information
# and automatically adds necessary 'moc' files as it sees Q_OBJECT macros' in header files
HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           qdeclarativelandmark_p.h \
           qdeclarativelandmarkmodel_p.h \
           qdeclarativelandmarkcategorymodel_p.h \
           qdeclarativelandmarkcategory_p.h \
           qdeclarativelandmarkfilters_p.h \
           qdeclarativecoordinate_p.h \
           qdeclarativegeomapobject_p.h \
           qdeclarativegeomapobjectborder_p.h \
           qdeclarativegeomapcircleobject_p.h \
           qdeclarativegeomappixmapobject_p.h \
           qdeclarativegeomappolygonobject_p.h \
           qdeclarativegeomappolylineobject_p.h \
           qdeclarativegeomaprectangleobject_p.h \
           qdeclarativegeomaptextobject_p.h \ 
           qdeclarativegeoserviceprovider_p.h \
           qdeclarativegraphicsgeomap_p.h \
           qdeclarativegeosearchmodel_p.h \
           qdeclarativegeocodemodel_p.h \
           qdeclarativereversegeocodemodel_p.h \
	   qdeclarativegeoplace_p.h \
	   qdeclarativegeoaddress_p.h \
	   qdeclarativegeoboundingbox_p.h

SOURCES += qdeclarativeposition.cpp \
           location.cpp \
           qdeclarativepositionsource.cpp \
           qdeclarativelandmark.cpp \
           qdeclarativelandmarkmodel.cpp \
           qdeclarativelandmarkcategorymodel.cpp \
           qdeclarativelandmarkcategory.cpp \
           qdeclarativelandmarkfilters.cpp \
           qdeclarativecoordinate.cpp \
           qdeclarativegeomapobject.cpp \
           qdeclarativegeomapobjectborder.cpp \
           qdeclarativegeomapcircleobject.cpp \
           qdeclarativegeomappixmapobject.cpp \
           qdeclarativegeomappolygonobject.cpp \
           qdeclarativegeomappolylineobject.cpp \
           qdeclarativegeomaprectangleobject.cpp \
           qdeclarativegeomaptextobject.cpp \
           qdeclarativegeoserviceprovider.cpp \
           qdeclarativegraphicsgeomap.cpp \
           qdeclarativegeosearchmodel.cpp \
           qdeclarativegeocodemodel.cpp \
           qdeclarativereversegeocodemodel.cpp \
	   qdeclarativegeoplace.cpp \
	   qdeclarativegeoaddress.cpp \
	   qdeclarativegeoboundingbox.cpp

# Tell qmake to create such makefile that qmldir and target (i.e. declarative_location)
# are both copied to qt/imports/QtMobility/location -directory,
# as the "/imports" is the default place where qmlviewer looks for plugins
# (otherwise qmlviewer -I <path> -option is needed)
TARGETPATH = QtMobility/location
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH
qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += qmldir

symbian {
    # In Symbian, a library should enjoy _largest_ possible capability set.
    # However, really needs only Location capability so feel free to reduce
    # depending on your signing capabilities.
    TARGET.CAPABILITY = ALL -TCB
    # Allow writable DLL data
    TARGET.EPOCALLOWDLLDATA = 1
    # Target UID, makes every Symbian app unique
    TARGET.UID3 = 0x20033007
    # Specifies what files shall be deployed: the plugin itself and the qmldir file.
    importFiles.sources = $$DESTDIR/declarative_location$${QT_LIBINFIX}.dll qmldir 
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    DEPLOYMENT = importFiles
 }
