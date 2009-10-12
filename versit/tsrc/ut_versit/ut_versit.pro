TEMPLATE = app
TARGET = 
QT += testlib xml
CONFIG += qtestlib
DEFINES += BUILD_QTVERSIT
DEPENDPATH += .
INCLUDEPATH += . ../../

HEADERS += testresultxmlparser.h \
    ../../qversitproperty.h \
    ../../qversitproperty_p.h \
    ut_qversitproperty.h \
    ../../qversitdocument.h \
    ../../qversitdocument_p.h \
    ut_qversitdocument.h \
    ../../qversitcontactgenerator.h \
    ut_qversitcontactgenerator.h \
    ut_versitutils.h \
    ../../qversitreader.h \
    ut_qversitreader.h \
    ../../qversitwriter.h \
    ut_qversitwriter.h \
        ../../qversitcontactconverter.h \
    ut_qversitcontactconverter.h 


SOURCES += main.cpp \
    testresultxmlparser.cpp \
    ../../qversitproperty.cpp \
    ut_qversitproperty.cpp \
    ../../qversitdocument.cpp \
    ut_qversitdocument.cpp \
    ../../qversitcontactgenerator.cpp \
    ut_qversitcontactgenerator.cpp \
    ../../versitutils.cpp \
    ut_versitutils.cpp \
    ../../qversitreader.cpp \
    ut_qversitreader.cpp \
    ../../qversitwriter.cpp \
    ut_qversitwriter.cpp \
        ../../qversitcontactconverter.cpp \
    ut_qversitcontactconverter.cpp

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl \
        -lqtcontacts
}
