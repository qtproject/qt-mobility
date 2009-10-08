TEMPLATE = app
TARGET = 
QT += testlib \
    xml
CONFIG += qtestlib
DEPENDPATH += .
INCLUDEPATH += . ../../

HEADERS += testresultxmlparser.h \
    ../../qversitproperty.h \
    ut_qversitproperty.h \
    ../../qversitdocument.h \
    ut_qversitdocument.h \
    ../../qversitcontactgenerator.h \
    ut_qversitcontactgenerator.h \
    ut_versitutils.h \
    ../../qversitreader.h \
    ut_qversitreader.h \

SOURCES += main.cpp \
    testresultxmlparser.cpp \
    ../../qversitproperty.cpp \
    ut_qversitproperty.cpp \
    ../../qversitdocument.cpp \
    ut_qversitdocument.cpp \
    ../../qversitcontactgenerator.cpp \
    ut_qversitcontactgenerator.cpp \
    ../../qversitcontactgenerator_p.cpp \
    ut_qversitcontactgenerator_p.cpp \
    ../../versitutils.cpp \
    ut_versitutils.cpp \
    ../../qversitreader.cpp \
    ut_qversitreader.cpp

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
