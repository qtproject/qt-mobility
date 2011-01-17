TEMPLATE = lib
CONFIG += plugin
QT += sql
TARGET = $$qtLibraryTarget(qtlandmarks_sqlite)
PLUGIN_TYPE=landmarks

include(../../../common.pri)

CONFIG += mobility
MOBILITY = location

HEADERS += qlandmarkmanagerengine_sqlite_p.h \
           qlandmarkmanagerenginefactory_sqlite_p.h \
           databasefilewatcher_p.h \
           databaseoperations_p.h \
           ../common/qlandmarkfilehandler_lmx_p.h \
           ../common/qlandmarkfilehandler_gpx_p.h \

SOURCES += qlandmarkmanagerengine_sqlite.cpp \
           qlandmarkmanagerenginefactory_sqlite.cpp \
           databasefilewatcher.cpp \
           databaseoperations.cpp \
           ../common/qlandmarkfilehandler_lmx.cpp \
           ../common/qlandmarkfilehandler_gpx.cpp

INCLUDEPATH += $$SOURCE_DIR/src/location \
                $$SOURCE_DIR/src/location/landmarks \
               ../common 

RESOURCES += qlandmarkmanagerengine_sqlite.qrc
