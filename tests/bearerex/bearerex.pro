TEMPLATE = app
TARGET = BearerEx

QT += core \
      gui \
      network

FORMS += detailedinfodialog.ui
maemo5|maemo6 {
    FORMS += sessiondialog_maemo.ui \
        bearerex_maemo.ui
} else {
    FORMS += sessiondialog.ui \
        bearerex.ui
}

include(../../common.pri)
#not really a test case but deployment happens same way
CONFIG += testcase

DEPENDPATH += .
INCLUDEPATH += . \
               ../../src/bearer

# Example headers and sources
HEADERS += bearerex.h \
           xqlistwidget.h
    
SOURCES += bearerex.cpp \
           main.cpp \
           xqlistwidget.cpp

CONFIG += mobility
MOBILITY = bearer
symbian:TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
