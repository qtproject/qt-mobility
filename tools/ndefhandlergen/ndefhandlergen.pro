QT       += core

QT       -= gui

TARGET = ndefhandlergen
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

RESOURCES += \
    templates.qrc

OTHER_FILES += \
    templates/symbian/symbian.xml \
    templates/maemo6/maemo6.conf \
    templates/maemo6/maemo6.service \
    templates/maemo6/maemo6.postinst \
    templates/maemo6/maemo6.prerm

include(../../common.pri)
include(../../features/deploy.pri)

