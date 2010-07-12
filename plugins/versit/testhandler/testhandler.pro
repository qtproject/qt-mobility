TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtversit_testhandler)
PLUGIN_TYPE=versit

include(../../../common.pri)

HEADERS += testvcardhandler.h
SOURCES += testvcardhandler.cpp

INCLUDEPATH += \
               $$SOURCE_DIR/src/contacts \
               $$SOURCE_DIR/src/contacts/details \
               $$SOURCE_DIR/src/contacts/filters \
               $$SOURCE_DIR/src/contacts/requests \
               $$SOURCE_DIR/src/versit
CONFIG += mobility
MOBILITY = versit contacts

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
INSTALLS += target
