TEMPLATE = app
CONFIG+=testcase
QT += xmlpatterns
TARGET=tst_qlandmarkfilehandler_lmx

include (../../../common.pri)

INCLUDEPATH += ../../../src/location

# Input 
HEADERS += ../qlandmarkcategory/mocklandmarkmanager.h
SOURCES += tst_qlandmarkfilehandler_lmx.cpp \
            ../qlandmarkcategory/mocklandmarkmanager.cpp

RESOURCES += data.qrc

CONFIG += mobility
MOBILITY = location

symbian {
    INCLUDEPATH += $${EPOCROOT}epoc32/include/osextensions
    TARGET.CAPABILITY = ALL -TCB
}
