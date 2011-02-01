TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qgeosatelliteinfosource

include (../../../../common.pri)

INCLUDEPATH += ../../../../src/location

SOURCES += tst_qgeosatelliteinfosource.cpp \
        ../testqgeosatelliteinfosource.cpp \
        ../../qgeocoordinate/qlocationtestutils.cpp \

HEADERS += ../testqgeosatelliteinfosource_p.h \
        ../../qgeocoordinate/qlocationtestutils_p.h \

CONFIG += mobility
MOBILITY = location

meego: contains (gypsy_enabled, yes) {
    CONFIG += qdbus link_pkgconfig
    PKGCONFIG += gconf-2.0 glib-2.0 gypsy
    QMAKE_PKGCONFIG_REQUIRES += glib-2.0 gconf-2.0 gypsy
    pkgconfig.path = $$QT_MOBILITY_LIB/pkgconfig
    pkgconfig.files = QtLocation.pc
}

symbian {
    TARGET.CAPABILITY = ALL -TCB
}
