TEMPLATE = app
TARGET = 
QT += testlib xml
CONFIG += qtestlib
DEFINES += BUILD_QTVERSIT QT_ASCII_CAST_WARNINGS
DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../ \
    ../../../src/versit \
    ../../../src/contacts \
    ../../../src/contacts/details \
    ../../../src/contacts/requests \
    ../../../src/contacts/filters

include(../../../common.pri)

HEADERS += testresultxmlparser.h \
    ../../../src/versit/qversitproperty.h \
    ../../../src/versit/qversitproperty_p.h \
    ut_qversitproperty.h \
    ../../../src/versit/qversitdocument.h \
    ../../../src/versit/qversitdocument_p.h \
    ut_qversitdocument.h \
    ../../../src/versit/qversitcontactimporter.h \
    ../../../src/versit/qversitcontactimporter_p.h \
    ut_qversitcontactimporter.h \
    ut_versitutils.h \
    ../../../src/versit/qversitreader.h \
    ut_qversitreader.h \
    ../../../src/versit/qversitwriter.h \
    ut_qversitwriter.h \
    ut_qvcard21writer.h \
    ut_qvcard30writer.h \
    ../../../src/versit/qversitcontactexporter.h \
    ../../../src/versit/qversitcontactexporter_p.h \
    ut_qversitcontactexporter.h 


SOURCES += main.cpp \
    testresultxmlparser.cpp \
    ../../../src/versit/qversitproperty.cpp \
    ut_qversitproperty.cpp \
    ../../../src/versit/qversitdocument.cpp \
    ut_qversitdocument.cpp \
    ../../../src/versit/qversitcontactimporter.cpp \
    ../../../src/versit/qversitcontactimporter_p.cpp \
    ut_qversitcontactimporter.cpp \
    ../../../src/versit/versitutils.cpp \
    ut_versitutils.cpp \
    ../../../src/versit/qversitreader.cpp \
    ../../../src/versit/qversitreader_p.cpp \
    ut_qversitreader.cpp \
    ../../../src/versit/qversitwriter.cpp \
    ../../../src/versit/qversitwriter_p.cpp \
    ut_qversitwriter.cpp \
    ../../../src/versit/qvcard21writer.cpp \
    ut_qvcard21writer.cpp \
    ../../../src/versit/qvcard30writer.cpp \
    ut_qvcard30writer.cpp \    
    ../../../src/versit/qversitcontactexporter.cpp \
    ../../../src/versit/qversitcontactexporter_p.cpp \
    ut_qversitcontactexporter.cpp

qtAddLibrary(QtContacts)

symbian: { 
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
