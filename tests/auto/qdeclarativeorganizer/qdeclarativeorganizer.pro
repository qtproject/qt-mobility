TEMPLATE=app

CONFIG += mobility testcase
MOBILITY += organizer versit
QT += testlib declarative

INCLUDEPATH += ../../../src/organizer
INCLUDEPATH += ../../../src/organizer/items
INCLUDEPATH += ../../../src/organizer/details
INCLUDEPATH += ../../../src/organizer/filters
INCLUDEPATH += ../../../src/organizer/requests
INCLUDEPATH += ../../../src/versit
INCLUDEPATH += ../../../src/versitorganizer
DEPENDPATH += ../../src/organizer
DEPENDPATH += ../../src/versit

include (../../../common.pri)

INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/organizer

SOURCES += tst_qdeclarativeorganizer.cpp


symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

