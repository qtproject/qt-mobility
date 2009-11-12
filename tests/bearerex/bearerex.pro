TEMPLATE = app
TARGET = BearerEx

QT += core \
      gui \
      network

FORMS += sessiondialog.ui \
         bearerex.ui \
         detailedinfodialog.ui
         
#not really a test case but deployment happens same way
CONFIG += testcase

DEPENDPATH += .
INCLUDEPATH += . \
               ../../bearer

# Example headers and sources
HEADERS += bearerex.h \
           xqlistwidget.h
    
SOURCES += bearerex.cpp \
           main.cpp \
           xqlistwidget.cpp

symbian: {
    bearerex.sources = Qtbearer.dll
    bearerex.path = /sys/bin
    DEPLOYMENT += bearerex

    qtAddLibrary(QtBearer)
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData
}
