TEMPLATE=app
INCLUDEPATH += ../../location

HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

CONFIG += console

include(../../common.pri)
LIBS += -lQtLocation

logfile.path = $$DESTDIR
logfile.files = simplelog.txt
INSTALLS += logfile
