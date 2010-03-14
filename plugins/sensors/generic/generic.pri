HEADERS += genericorientationsensor.h

SOURCES += genericorientationsensor.cpp\
           main.cpp\

!symbian:!win* {
    HEADERS += genericattitudesensor.h
    SOURCES += genericattitudesensor.cpp
    LIBS += -lm
}

