TEMPLATE = app
CONFIG+=testcase
QT += network

include (../../../common.pri)

INCLUDEPATH += ../../../location

HEADERS += ../qlocationtestutils_p.h \
           ../testqgeopositioninfosource_p.h \
           qnmeapositioninfosourceproxyfactory.h

SOURCES += ../qlocationtestutils.cpp \
           ../testqgeopositioninfosource.cpp \
           qnmeapositioninfosourceproxyfactory.cpp \
           tst_qnmeapositioninfosource.cpp

LIBS += -lQtLocation

symbian {
        TARGET.CAPABILITY = ALL -TCB
}

symbian {
    logfile.path = ./tst_qnmeapositioninfosource_nmealog.txt
} else {
    logfile.path = $$DESTDIR
}
logfile.files = tst_qnmeapositioninfosource_nmealog.txt
logfile.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += logfile
build_pass:ALL_DEPS+=install_logfile
