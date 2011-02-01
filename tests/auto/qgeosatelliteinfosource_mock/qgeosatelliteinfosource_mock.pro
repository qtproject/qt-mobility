TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfosource_mock

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

SOURCES += ../qgeosatelliteinfosource/test/tst_qgeosatelliteinfosource.cpp \
        ../qgeosatelliteinfosource/testqgeosatelliteinfosource.cpp \
        ../qgeocoordinate/qlocationtestutils.cpp \

HEADERS += ../qgeosatelliteinfosource/testqgeosatelliteinfosource_p.h \
        ../qgeocoordinate/qlocationtestutils_p.h \

CONFIG += mobility
MOBILITY = location

meego: contains (gypsy_enabled, yes) {
    DEFINES += TST_GYPSYMOCK_ENABLED=1
    message("Mocking Gypsy (not using real libraries).")
    INCLUDEPATH += ../qgeosatelliteinfosource/gypsymock
    RESOURCES += ../qgeosatelliteinfosource/gypsymock/gypsy_mock_data.qrc
    # needs to be compiled because we use private symbols (init):
    HEADERS += ../../../src/location/qgeosatelliteinfosource_gypsy_p.h
    HEADERS += ../qgeosatelliteinfosource/gypsymock/gypsymock.h
    SOURCES += ../../../src/location/qgeosatelliteinfosource_gypsy.cpp
    SOURCES += ../qgeosatelliteinfosource/gypsymock/gypsymock.cpp
    CONFIG += qdbus link_pkgconfig
    PKGCONFIG += gconf-2.0 glib-2.0 gypsy
    QMAKE_PKGCONFIG_REQUIRES += glib-2.0 gconf-2.0 gypsy
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtLocation.pc
} else {
    message("Gypsy not available, unable to mock the backend.")
}

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
