TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtcontacts_maemo5)
PLUGIN_TYPE=contacts

include(../../../common.pri)

HEADERS += \
        qcontactidshash.h \
        qcontactabook_p.h \
        qcontactmaemo5backend_p.h \
        qcontactmaemo5debug_p.h 
SOURCES += \
        qcontactabook.cpp \
        qcontactmaemo5backend.cpp \

INCLUDEPATH += $$SOURCE_DIR/src/contacts $$SOURCE_DIR/src/contacts/details $$SOURCE_DIR/src/contacts/filters $$SOURCE_DIR/src/contacts/requests

target.path=$${QT_MOBILITY_PREFIX}/plugins/$${PLUGIN_TYPE}
maemo6:target.path=$$[QT_INSTALL_PLUGINS]/$${PLUGIN_TYPE}
INSTALLS += target
CONFIG += mobility link_pkgconfig
MOBILITY = contacts
PKGCONFIG += libosso-abook-1.0
