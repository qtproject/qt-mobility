TEMPLATE=app
TARGET=servicexmlgen

INCLUDEPATH += ../../src/serviceframework

HEADERS = servicexmlgen.h \
          servicewidget.h \
          interfacestabwidget.h \
          interfacewidget.h \
          mandatorylineedit.h \
          errorcollector.h

SOURCES = servicexmlgen.cpp \
          servicewidget.cpp \
          interfacestabwidget.cpp \
          interfacewidget.cpp \
          mandatorylineedit.cpp \
          errorcollector.cpp

include(../../common.pri)
qtAddLibrary(QtServiceFramework)

include(../../features/deploy.pri)
