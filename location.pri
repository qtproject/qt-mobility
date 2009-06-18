CONFIG += debug_and_release build_all

# On win32 and mac, debug and release libraries are named differently.
# We must follow the debug and release settings Qt was compiled with:
# build debug iff Qt built debug, build release iff Qt built release.
win32|mac {
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug):  CONFIG+=debug
        contains(QT_CONFIG,release):CONFIG+=release
    }
}

QT += core

DEFINES += QT_BUILD_LOCATION_LIB QT_MAKEDLL

INCLUDEPATH += $$PWD/src/

HEADERS +=  $$PWD/src/qlocationglobal.h \
            $$PWD/src/qcoordinate.h

SOURCES +=  $$PWD/src/qcoordinate.cpp

