QT += testlib
TEMPLATE=app
TARGET=tst_qversitcontactplugins
CONFIG+=testcase
CONFIG+=parallel_test

include(../../../../common.pri)

DEPENDPATH += .
INCLUDEPATH += \
    . \
    ../../../ \
    ../../../../src/versit \
    ../../../../src/contacts \
    ../../../../src/contacts/details \
    ../../../../src/contacts/requests \
    ../../../../src/contacts/filters

SOURCES += tst_qversitcontactplugins.cpp

CONFIG += mobility
MOBILITY += versit

symbian: {
    TARGET.CAPABILITY = ALL \
        -TCB
    LIBS += -lws32 \
        -lbafl
}
