TARGET = sfwecho_client2
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework
include(../../examples.pri)
QT = core \
    gui
TEMPLATE = app
CONFIG += mobility
MOBILITY = serviceframework
HEADERS += 
SOURCES += ../sfwecho_client/main.cpp
symbian { 
    TARGET.CAPABILITY = ALL \
        -TCB
    FORMS += ../sfwecho_client/sfwecho_client_mobile.ui
    INCLUDEPATH += .
}
else:FORMS += ../sfwecho_client/sfwecho_client.ui
