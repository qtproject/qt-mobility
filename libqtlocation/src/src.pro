TEMPLATE = lib
CONFIG += dll debug
TARGET = QtLocation
DEPENDPATH += .
MOC_DIR = .moc
OBJECTS_DIR = .obj
INCLUDEPATH += .
QMAKE_CXXFLAGS += -g -Wall -Wextra -Werror

CONFIG += qdbus
DEFINES += Q_WS_MAEMO_6
DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

# Header files
HEADERS += qgeocoordinate.h qgeopositioninfo.h qgeopositioninfosource.h\
           qgeopositioninfosource_maemo_p.h dbuscomm_maemo_p.h dbusserver_maemo_p.h\
           qgeosatelliteinfo.h qgeosatelliteinfosource.h qgeosatelliteinfosource_maemo_p.h

# Source files
SOURCES += qgeocoordinate.cpp qgeopositioninfo.cpp qgeopositioninfosource.cpp\
           qgeopositioninfosource_maemo.cpp dbuscomm_maemo.cpp dbusserver_maemo.cpp\
           qgeosatelliteinfo.cpp qgeosatelliteinfosource.cpp\
           qgeosatelliteinfosource_maemo.cpp

QMAKE_DISTCLEAN += -r ../build-stamp ../configure-stamp .moc .obj

# Install
target.path += $$[QT_INSTALL_LIBS]

# Install headers that make up the public API
headers.path += $$[QT_INSTALL_HEADERS]/$$TARGET
headers.files += qgeocoordinate.h qgeopositioninfo.h qgeopositioninfosource.h qmobilityglobal-tmp.h \
                 qgeosatelliteinfo.h qgeosatelliteinfosource.h

# install pkgconfig file(s)
pkgconfigfiles.path  += /usr/lib/pkgconfig/
pkgconfigfiles.files += QtLocation.pc

# Install instructions
INSTALLS += target \
            headers \
	    pkgconfigfiles
	    
