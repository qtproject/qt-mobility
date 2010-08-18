TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtversit_backuphandler)
PLUGIN_TYPE=versit

include(../../../common.pri)

HEADERS += backupvcardhandler.h
SOURCES += backupvcardhandler.cpp

INCLUDEPATH += \
               $$SOURCE_DIR/src/contacts \
               $$SOURCE_DIR/src/contacts/details \
               $$SOURCE_DIR/src/contacts/filters \
               $$SOURCE_DIR/src/contacts/requests \
               $$SOURCE_DIR/src/versit
CONFIG += mobility
MOBILITY = versit contacts

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
}
