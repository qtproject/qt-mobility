TEMPLATE = app
TARGET = BearerEx

QT += core \
      gui \
      network

FORMS += sessiondialog.ui \
         bearerex.ui \
         detailedinfodialog.ui
         
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
    bearerex.sources = bearer.dll
    bearerex.path = /sys/bin
    DEPLOYMENT += bearerex

    LIBS += -lbearer
    TARGET.CAPABILITY = NetworkServices NetworkControl ReadUserData 
}
