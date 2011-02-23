TARGET = sfwecho_client
INCLUDEPATH += ../../../src/serviceframework
DEPENDPATH += ../../../src/serviceframework
include(../../examples.pri)
QT = core \
    gui
TEMPLATE = app
CONFIG += mobility
MOBILITY = serviceframework
HEADERS += 
SOURCES += main.cpp

symbian|maemo*|wince* {
    FORMS += sfwecho_client_mobile.ui
}
else {
    FORMS += sfwecho_client.ui
}
