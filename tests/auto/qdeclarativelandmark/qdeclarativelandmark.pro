TEMPLATE=app
CONFIG+=testcase
include (../../../common.pri)
# prefix test binary with "tst_"
!contains(TARGET, ^tst_.*):TARGET = $$join(TARGET,,"tst_")
QT += testlib declarative

INCLUDEPATH += ../../../src/location
INCLUDEPATH += ../../../plugins/declarative/location
INCLUDEPATH += ../../../src/location/landmarks
INCLUDEPATH += ../../../src/location/maps
INCLUDEPATH += ../../../src/global

HEADERS += ../../../plugins/declarative/location/qdeclarativeposition_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativepositionsource_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativelandmarkcategorymodel_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativelandmarkcategory_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativelandmarkfilters_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativelandmarkmodel_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativelandmark_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativecoordinate_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativegeoaddress_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativegeoplace_p.h
HEADERS += ../../../plugins/declarative/location/qdeclarativegeoboundingbox_p.h

SOURCES += tst_qdeclarativelandmark.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativeposition.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativepositionsource.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativelandmarkcategory.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativelandmark.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativelandmarkcategorymodel.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativelandmarkfilters.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativelandmarkmodel.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativecoordinate.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativegeoaddress.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativegeoplace.cpp
SOURCES += ../../../plugins/declarative/location/qdeclarativegeoboundingbox.cpp

CONFIG += mobility
MOBILITY += location

maemo6|meego {
    DEFINES+=SPARQL_BACKEND
}

symbian|wince* {
    symbian {
        TARGET.CAPABILITY = All -TCB
        TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    }
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

