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
    templates/harmattan/harmattan.conf \
    templates/harmattan/harmattan.service \
    templates/harmattan/harmattan.postinst \
    templates/harmattan/harmattan.prerm

include(../../common.pri)
include(../../features/deploy.pri)

