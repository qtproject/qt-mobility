TEMPLATE = app
TARGET = 
QT += testlib \
    xml
CONFIG += qtestlib
DEPENDPATH += .
INCLUDEPATH += . ../../

HEADERS += testresultxmlparser.h \
    ../../qversitcontactgenerator.h \
    ut_qversitcontactgenerator.h \
    ../../qversitcontactgenerator_p.h \
    ut_qversitcontactgenerator_p.h \
    ut_versitutils.h

SOURCES += main.cpp \
    testresultxmlparser.cpp \
    ../../qversitcontactgenerator.cpp \
    ut_qversitcontactgenerator.cpp \
    ../../qversitcontactgenerator_p.cpp \
    ut_qversitcontactgenerator_p.cpp \
    ../../versitutils.cpp \
    ut_versitutils.cpp

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
