HEADERS += $$PWD/graphicsmemorymonitor.h
SOURCES += $$PWD/graphicsmemorymonitor.cpp
INCLUDEPATH += $$PWD
DEFINES += GRAPHICSMEMORYMONITOR_SUPPORT
symbian {
    contains(QT_CONFIG, egl) {
        DEFINES += GRAPHICSMEMORYMONITOR_EGL
        LIBS += -llibegl
    }
}
message("graphicsmemorymonitor.pri loaded")

