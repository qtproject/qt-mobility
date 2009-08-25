TEMPLATE=app
INCLUDEPATH += ../../location

QT += network webkit 

HEADERS = mapwindow.h
SOURCES = mapwindow.cpp \
          main.cpp

include(../../common.pri)
LIBS += -lQtLocation

logfile.path = $$DESTDIR
logfile.files = nmealog.txt
INSTALLS += logfile
