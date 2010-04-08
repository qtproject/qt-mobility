TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkfilehandler_gpx

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlandmarkcategory/mocklandmarkmanager.h
SOURCES += tst_qlandmarkfilehandler_gpx.cpp \
            ../qlandmarkcategory/mocklandmarkmanager.cpp

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
