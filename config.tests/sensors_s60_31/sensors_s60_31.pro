CONFIG -= qt
TEMPLATE = app

# Input
SOURCES += main.cpp

INCLUDEPATH += \epoc32\include\app
INCLUDEPATH += \epoc32\include\platform\app
INCLUDEPATH += \epoc32\include\app\loc 
LIBS += -lRRSensorApi

