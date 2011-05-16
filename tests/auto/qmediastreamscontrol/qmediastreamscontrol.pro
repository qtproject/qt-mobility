include(../multimedia_common.pri)

TARGET = tst_qmediastreamscontrol

SOURCES += \
        tst_qmediastreamscontrol.cpp

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}