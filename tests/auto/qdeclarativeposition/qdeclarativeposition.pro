TEMPLATE=app
CONFIG+=testcase
include (../../../common.pri)
# prefix test binary with "tst_"
!contains(TARGET, ^tst_.*):TARGET = $$join(TARGET,,"tst_")
QT += testlib declarative

SOURCES += tst_qdeclarativeposition.cpp

# The plugin needs to be compiled as part of this test case because 
# it is not a library you could directly link against, ie. most of the API is not exported :)
SOURCES += ../../../plugins/declarative/location/qdeclarativeposition.cpp
HEADERS += ../../../plugins/declarative/location/qdeclarativeposition.h
INCLUDEPATH += ../../../src/location
CONFIG += mobility
MOBILITY += location

INCLUDEPATH += ../../../plugins/declarative/location

symbian {
    TARGET.CAPABILITY = Location
    TARGET.EPOCHEAPSIZE = 0x100000 0x2000000
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}
