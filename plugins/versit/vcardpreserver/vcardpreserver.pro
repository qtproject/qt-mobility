TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtversit_vcardpreserver)
PLUGIN_TYPE=versit

include(../../../common.pri)

HEADERS += vcardpreserver.h
SOURCES += vcardpreserver.cpp

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
    TARGET.CAPABILITY = ALL -Tcb
    TARGET.UID3 = 0x20031602

    target.path = /sys/bin
    INSTALLS += target

    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = $${QT_PLUGINS_BASE_DIR}/$${PLUGIN_TYPE}
    DEPLOYMENT += symbianplugin
}
