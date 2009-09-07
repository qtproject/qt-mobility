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

logfile.path = $$DESTDIR
logfile.files = tst_qnmeapositioninfosource_nmealog.txt
INSTALLS += logfile
