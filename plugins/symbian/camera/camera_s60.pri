INCLUDEPATH += $$PWD

symbian:LIBS += -lcamerawrapper \
    -lfbscli

HEADERS += \
    $$PWD/s60cameracontrol.h \
    $$PWD/s60mediacontrol.h \
    $$PWD/s60camerasession.h \ 
    $$PWD/s60mediaformatcontrol.h \
    $$PWD/s60videoencoder.h \
    $$PWD/s60videooutputcontrol.h \
    $$PWD/s60cameraservice.h \
#    $$PWD/s60videowidget.h

SOURCES += $$PWD/s60cameracontrol.cpp \
    $$PWD/s60mediacontrol.cpp \
    $$PWD/s60camerasession.cpp \
    $$PWD/s60mediaformatcontrol.cpp \
    $$PWD/s60videoencoder.cpp \
    $$PWD/s60videooutputcontrol.cpp \
    $$PWD/s60cameraservice.cpp \
#    $$PWD/s60videowidget.cpp
