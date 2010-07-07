INCLUDEPATH += effects

unix:!mac:!symbian {
   contains(QT_CONFIG, pulseaudio) {
        DEFINES += QT_MULTIMEDIA_PULSEAUDIO
        PRIVATE_HEADERS += effects/qsoundeffect_pulse_p.h
        SOURCES += effects/qsoundeffect_pulse_p.cpp
        QMAKE_CXXFLAGS += $$QT_CFLAGS_PULSEAUDIO
        LIBS += $$QT_LIBS_PULSEAUDIO
    } else {
        DEFINES += QT_MULTIMEDIA_QMEDIAPLAYER
        PRIVATE_HEADERS += effects/qsoundeffect_qmedia_p.h
        SOURCES += effects/qsoundeffect_qmedia_p.cpp
    }
} else {
    PRIVATE_HEADERS += effects/qsoundeffect_qsound_p.h
    SOURCES += effects/qsoundeffect_qsound_p.cpp
}

PRIVATE_HEADERS += effects/wavedecoder_p.h
PUBLIC_HEADERS += effects/qsoundeffect.h

SOURCES += \
    effects/qsoundeffect.cpp \
    effects/wavedecoder_p.cpp
