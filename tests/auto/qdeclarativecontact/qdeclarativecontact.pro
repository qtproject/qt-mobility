load(qttest_p4)
include (../../../common.pri)
CONFIG += mobility 
MOBILITY += contacts versit
QT += declarative

INCLUDEPATH += ../../../src/contacts
INCLUDEPATH += ../../../src/contacts/details
INCLUDEPATH += ../../../src/contacts/filters
INCLUDEPATH += ../../../src/contacts/requests
INCLUDEPATH += ../../../src/versit
DEPENDPATH += ../../src/contacts
DEPENDPATH += ../../src/versit


INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/contacts
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/contacts/details
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/contacts/filters

SOURCES += tst_qdeclarativecontact.cpp


symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

