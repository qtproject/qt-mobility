requires(contains(QT_CONFIG,webkit))

TEMPLATE=app
INCLUDEPATH += ../../location

QT += webkit 

HEADERS = mapwindow.h
SOURCES = mapwindow.cpp \
          main.cpp

include(../../common.pri)
LIBS += -lQtLocation

logfile.path = $$DESTDIR
logfile.files = nmealog.txt
INSTALLS += logfile
