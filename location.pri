QT += core


DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL


INCLUDEPATH += $$PWD/src/

HEADERS +=  $$PWD/src/qlocationglobal.h \
            $$PWD/src/qcoordinate.h

SOURCES +=  $$PWD/src/qcoordinate.cpp

