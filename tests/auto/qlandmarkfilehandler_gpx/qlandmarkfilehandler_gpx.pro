TEMPLATE = app
CONFIG+=testcase
TARGET=tst_qlandmarkfilehandler_gpx

include (../../../common.pri)

QT += sql

INCLUDEPATH += ../../../src/location \
               ../../../src/location/landmarks \
               ../../../plugins/landmarks/sqlite \
               ../

# Input 
SOURCES += tst_qlandmarkfilehandler_gpx.cpp \
           ../../../plugins/landmarks/sqlite/qlandmarkfilehandler_gpx.cpp

HEADERS += ../../../plugins/landmarks/sqlite/qlandmarkfilehandler_gpx_p.h \
           ../qlandmarkmanagerdataholder.h

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
