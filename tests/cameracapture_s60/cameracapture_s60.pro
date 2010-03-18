TEMPLATE = app
TARGET = s60cameracapture

INCLUDEPATH += ../../src/multimedia
include(../../examples/examples.pri)

CONFIG += mobility
MOBILITY = multimedia

HEADERS = cameracapture.h \
    settings.h \
    stillsettings.h
SOURCES = main.cpp \
    cameracapture.cpp \
    settings.cpp \
    stillsettings.cpp

symbian {
    FORMS += cameracapture.ui \
        settings_s60.ui \
        stillsettings_s60.ui
} else:maemo5 {
    HEADERS += previewdialog.h
    SOURCES += previewdialog.cpp
    FORMS += previewdialog_maemo.ui \
	     cameracapture_maemo.ui \
	     settings_s60.ui \
	     stillsettings_s60.ui
} else {
    FORMS += cameracapture.ui \
        settings.ui
}

symbian: {
    TARGET.CAPABILITY = UserEnvironment WriteDeviceData ReadDeviceData MultimediaDD
    include(mediakeysobserver.pri)
}

