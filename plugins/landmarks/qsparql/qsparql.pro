TEMPLATE = lib
CONFIG += plugin
QT += sql
QT += dbus
TARGET = $$qtLibraryTarget(qtlandmarks_qsparql)
PLUGIN_TYPE=landmarks

include(../../../common.pri)

CONFIG += mobility link_pkgconfig
MOBILITY = location

LIBS += -lQtSparql

HEADERS += qlandmarkmanagerengine_qsparql_p.h \
           qlandmarkmanagerenginefactory_qsparql_p.h \
           databaseoperations_p.h \
           qlandmarkfilehandler_lmx_p.h \
           qlandmarkfilehandler_gpx_p.h \
           trackernotifier_p.h \
           trackernotifier_signals.h


SOURCES += qlandmarkmanagerengine_qsparql.cpp \
           qlandmarkmanagerenginefactory_qsparql.cpp \
           databaseoperations.cpp \
           qlandmarkfilehandler_lmx.cpp \
           qlandmarkfilehandler_gpx.cpp \
           trackernotifier.cpp

INCLUDEPATH += $$SOURCE_DIR/src/location \
                $$SOURCE_DIR/src/location/landmarks

RESOURCES += qlandmarkmanagerengine_qsparql.qrc

DEFINES += QSPARQL

PKGCONFIG += QtSparql
