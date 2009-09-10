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
logfile.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += logfile
build_pass:ALL_DEPS+=install_logfile
