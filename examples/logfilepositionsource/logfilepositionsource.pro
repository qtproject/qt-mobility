TEMPLATE=app
INCLUDEPATH += ../../location

HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

CONFIG += console

include(../examples.pri)
LIBS += -lQtLocation

logfile.path = $$DESTDIR
logfile.files = simplelog.txt
INSTALLS += logfile
build_pass:ALL_DEPS+=install_logfile
