TEMPLATE = app
TARGET = 
QT += testlib xml
CONFIG += qtestlib
DEFINES += BUILD_QTVERSIT QT_ASCII_CAST_WARNINGS
DEPENDPATH += .
INCLUDEPATH += . ../../

include(../../../common.pri)

HEADERS += testresultxmlparser.h \
    ../../qversitproperty.h \
    ../../qversitproperty_p.h \
    ut_qversitproperty.h \
    ../../qversitdocument.h \
    ../../qversitdocument_p.h \
    ut_qversitdocument.h \
    ../../qversitcontactimporter.h \
    ../../qversitcontactimporter_p.h \
    ut_qversitcontactimporter.h \
    ut_versitutils.h \
    ../../qversitreader.h \
    ut_qversitreader.h \
    ../../qversitwriter.h \
    ut_qversitwriter.h \
    ut_qvcard21writer.h \
    ut_qvcard30writer.h \
    ../../qversitcontactexporter.h \
    ../../qversitcontactexporter_p.h \
    ut_qversitcontactexporter.h 


SOURCES += main.cpp \
    testresultxmlparser.cpp \
    ../../qversitproperty.cpp \
    ut_qversitproperty.cpp \
    ../../qversitdocument.cpp \
    ut_qversitdocument.cpp \
    ../../qversitcontactimporter.cpp \
    ../../qversitcontactimporter_p.cpp \
    ut_qversitcontactimporter.cpp \
    ../../versitutils.cpp \
    ut_versitutils.cpp \
    ../../qversitreader.cpp \
    ../../qversitreader_p.cpp \
    ut_qversitreader.cpp \
    ../../qversitwriter.cpp \
    ../../qversitwriter_p.cpp \
    ut_qversitwriter.cpp \
    ../../qvcard21writer.cpp \
    ut_qvcard21writer.cpp \
    ../../qvcard30writer.cpp \
    ut_qvcard30writer.cpp \    
    ../../qversitcontactexporter.cpp \
    ../../qversitcontactexporter_p.cpp \
    ut_qversitcontactexporter.cpp

qtAddLibrary(QtContacts)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
