INCLUDEPATH += $$PWD \
               ./

HEADERS += qaudio.h \
           qaudioformat.h \
           qaudioinput.h \
           qaudiooutput.h \
           qaudiodeviceinfo.h \
           qaudiosystemplugin.h \
           qaudiosystem.h \
           qaudiodevicefactory_p.h


SOURCES += qaudio.cpp \
           qaudioformat.cpp  \
           qaudiodeviceinfo.cpp \
           qaudiooutput.cpp \
           qaudioinput.cpp \
           qaudiosystemplugin.cpp \
           qaudiosystem.cpp \
           qaudiodevicefactory.cpp

#contains(QT_CONFIG, audio-backend) {

mac {
    HEADERS +=  qaudioinput_mac_p.h \
                qaudiooutput_mac_p.h \
                qaudiodeviceinfo_mac_p.h \
                qaudio_mac_p.h

    SOURCES += qaudiodeviceinfo_mac_p.cpp \
               qaudiooutput_mac_p.cpp \
               qaudioinput_mac_p.cpp \
               qaudio_mac.cpp

    LIBS += -framework ApplicationServices -framework CoreAudio -framework AudioUnit -framework AudioToolbox

} else:win32 {

    HEADERS += qaudioinput_win32_p.h qaudiooutput_win32_p.h qaudiodeviceinfo_win32_p.h
    SOURCES += qaudiodeviceinfo_win32_p.cpp \
               qaudiooutput_win32_p.cpp \
               qaudioinput_win32_p.cpp
    !wince*:LIBS += -lwinmm
    wince*:LIBS += -lcoredll

} else:symbian {
    INCLUDEPATH += /epoc32/include/mmf/common
    INCLUDEPATH += /epoc32/include/mmf/server

    HEADERS += qaudio_symbian_p.h \
               qaudiodeviceinfo_symbian_p.h \
               qaudioinput_symbian_p.h \
               qaudiooutput_symbian_p.h

    SOURCES += qaudio_symbian_p.cpp \
               qaudiodeviceinfo_symbian_p.cpp \
               qaudioinput_symbian_p.cpp \
               qaudiooutput_symbian_p.cpp

    LIBS += -lmmfdevsound
} else:unix {
    unix:contains(QT_CONFIG, alsa) {
        linux-*|freebsd-*|openbsd-*:{
            DEFINES += HAS_ALSA
            HEADERS += qaudiooutput_alsa_p.h qaudioinput_alsa_p.h qaudiodeviceinfo_alsa_p.h
            SOURCES += qaudiodeviceinfo_alsa_p.cpp \
                   qaudiooutput_alsa_p.cpp \
                   qaudioinput_alsa_p.cpp
            LIBS_PRIVATE += -lasound
        }
    }
}
#} else {
#    DEFINES += QT_NO_AUDIO_BACKEND
#}
