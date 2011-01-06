TEMPLATE = lib
CONFIG += plugin
QT += sql
QT += dbus
TARGET = $$qtLibraryTarget(qtlandmarks_qsparql)
PLUGIN_TYPE=landmarks

include(../../../common.pri)

CONFIG += mobility link_pkgconfig
MOBILITY = location

LIBS += -lQtSparql \
           -lqtsparqltrackerextensions

HEADERS += qlandmarkmanagerengine_qsparql_p.h \
           qlandmarkmanagerenginefactory_qsparql_p.h \
           databaseoperations_p.h \
           ../common/qlandmarkfilehandler_lmx_p.h \
           ../common/qlandmarkfilehandler_gpx_p.h

SOURCES += qlandmarkmanagerengine_qsparql.cpp \
           qlandmarkmanagerenginefactory_qsparql.cpp \
           databaseoperations.cpp \
           ../common/qlandmarkfilehandler_lmx.cpp \
           ../common/qlandmarkfilehandler_gpx.cpp

INCLUDEPATH += $$SOURCE_DIR/src/location \
                $$SOURCE_DIR/src/location/landmarks \
               ../common

RESOURCES += qlandmarkmanagerengine_qsparql.qrc

DEFINES += QSPARQL

PKGCONFIG += QtSparql
