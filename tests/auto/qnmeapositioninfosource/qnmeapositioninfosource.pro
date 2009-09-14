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

DEFINES += \'_STR(X)=\\$${LITERAL_HASH}X\'
DEFINES += \'STR(X)=_STR(X)\'
DEFINES += \'_LOGDIR=$$DESTDIR\'
DEFINES += \'LOGDIR=STR(_LOGDIR)\'


logfile.path = $$DESTDIR
logfile.files = tst_qnmeapositioninfosource_nmealog.txt
logfile.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
INSTALLS += logfile
build_pass:ALL_DEPS+=install_logfile
