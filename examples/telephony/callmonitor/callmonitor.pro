TARGET = callmonitor
TEMPLATE = app


SOURCES += main.cpp\
        monitordialog.cpp \

HEADERS  += monitordialog.h

INCLUDEPATH += ../../../src/telephony
FORMS += dialog.ui

LIBS += -lQtTelephony


include(../../examples.pri)
CONFIG += mobility
MOBILITY = telephony
