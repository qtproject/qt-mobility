TEMPLATE=app
TARGET=declarative-sfw-dialer
INCLUDEPATH += ../../../src/serviceframework

include(../../examples.pri)

QT += gui declarative

# Input 
HEADERS += sfwexample.h
SOURCES += sfwexample.cpp \
           main.cpp

CONFIG += mobility
MOBILITY = serviceframework

RESOURCES += resource.qrc

symbian {
    # There are platsec warnings about missing 'AllFiles' 
    # capabilities, but application works regardless 
    # (they come from QDeclarativeView::setSource).
    # The unnecessary need for 'AllFiles' is being analyzed.
    # Comment date: 18-Mar-2010
    TARGET.CAPABILITY = ReadUserData WriteUserData
}
