TEMPLATE=app
TARGET=servicexmlgen
CONFIG+=console

INCLUDEPATH += ../../serviceframework

HEADERS = servicewidget.h \
          interfacestabwidget.h \
          interfacewidget.h \
          mandatorylineedit.h
SOURCES = servicexmlgen.cpp \
          servicewidget.cpp \
          interfacestabwidget.cpp \
          interfacewidget.cpp \
          mandatorylineedit.cpp

include(../../common.pri)
LIBS += -lQtServiceFramework
