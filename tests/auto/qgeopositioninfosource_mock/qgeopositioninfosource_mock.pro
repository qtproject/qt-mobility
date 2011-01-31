TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeopositioninfosource_mock

include (../../../common.pri)

INCLUDEPATH += ../../../src/location
INCLUDEPATH += ../qgeopositioninfosource
DEPENDPATH += ../../../buid/Debug/bin

# Input 
HEADERS += ../qgeocoordinate/qlocationtestutils_p.h \
           ../qgeopositioninfosource/testqgeopositioninfosource_p.h

SOURCES += ../qgeocoordinate/qlocationtestutils.cpp \
           ../qgeopositioninfosource/testqgeopositioninfosource.cpp \
           ../qgeopositioninfosource/tst_qgeopositioninfosource.cpp

# With geoclue mock the underlying layers. For this purpose do not
# link against the mobility location library.
meego: contains (geoclue-master_enabled, yes) {
    message("Mocking GeoClue libraries (real libraries will not be used)")
    DEFINES += TST_GEOCLUEMOCK_ENABLED=1
    DEFINES += GEOCLUE_MASTER_AVAILABLE=1

    HEADERS +=   ../../../src/location/qgeopositioninfosource_geocluemaster_p.h \
                 ../../../src/location/qgeopositioninfo.h \
                 ../../../src/location/qgeopositioninfosource.h \
                 ../../../src/location/qgeocoordinate.h \
                 ../qgeopositioninfosource/geocluemock.h

    HEADERS += ../../../src/location/qgeopositioninfosource_maemo_p.h \
                ../../../src/location/dbuscomm_maemo_p.h \
                ../../../src/location/dbusserver_maemo_p.h \
                ../../../src/location/qgeosatelliteinfo.h

    SOURCES +=   ../../../src/location/qgeopositioninfosource_geocluemaster.cpp \
                 ../../../src/location/qgeopositioninfo.cpp \
                 ../../../src/location/qgeopositioninfosource.cpp \
                 ../../../src/location/qgeocoordinate.cpp \
                 ../qgeopositioninfosource/geocluemock.cpp

    SOURCES += ../../../src/location/qgeopositioninfosource_maemo.cpp \
                ../../../src/location/dbuscomm_maemo.cpp \
                ../../../src/location/dbusserver_maemo.cpp \
                ../../../src/location/qgeosatelliteinfo.cpp

    RESOURCES += ../qgeopositioninfosource/geoclue_mock_data.qrc
    PKGCONFIG += geoclue gconf-2.0 glib-2.0
    QMAKE_PKGCONFIG_REQUIRES += geoclue glib-2.0 gconf-2.0
    CONFIG += qdbus link_pkgconfig
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtLocation.pc
} else {
    message("Unable to mock GeoClue libraries (are you building for meego and have geoclue devel libs?")
    CONFIG += mobility
    MOBILITY = location
}

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

CONFIG += middleware
include(../../../features/deploy.pri)
