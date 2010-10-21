TEMPLATE = lib
CONFIG += plugin
QT += sql
TARGET = $$qtLibraryTarget(qtlandmarks_qsparql)
PLUGIN_TYPE=landmarks

include(../../../common.pri)

CONFIG += mobility link_pkgconfig
MOBILITY = location

#LIBS += ../../../lib/libQtLocation.so \
LIBS += -lQtSparql

HEADERS += qlandmarkmanagerengine_qsparql_p.h \
           qlandmarkmanagerenginefactory_qsparql_p.h \
           databaseoperations_p.h \
           qlandmarkfilehandler_lmx_p.h \
           qlandmarkfilehandler_gpx_p.h \


SOURCES += qlandmarkmanagerengine_qsparql.cpp \
           qlandmarkmanagerenginefactory_qsparql.cpp \
           databaseoperations.cpp \
           qlandmarkfilehandler_lmx.cpp \
           qlandmarkfilehandler_gpx.cpp

INCLUDEPATH += $$SOURCE_DIR/src/location \
                $$SOURCE_DIR/src/location/landmarks

RESOURCES += qlandmarkmanagerengine_qsparql.qrc

DEFINES += QSPARQL

PKGCONFIG += QtSparql
