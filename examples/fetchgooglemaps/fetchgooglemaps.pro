TEMPLATE=app
INCLUDEPATH += ../../location

QT += webkit 

HEADERS = mapwindow.h
SOURCES = mapwindow.cpp \
          main.cpp

include(../examples.pri)
LIBS += -lQtLocation

logfile.path = $$DESTDIR
logfile.files = nmealog.txt
INSTALLS += logfile
build_pass:ALL_DEPS+=install_logfile
