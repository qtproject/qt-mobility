include(../ut_qtcontacts_common/ut_qtcontacts_common.pri)

TARGET = ut_qtcontacts_fetch

test.depends = all
QMAKE_EXTRA_TARGETS += test
QCONTACTS_TRACKER_BACKENDDIR = ../../

## Include unit test files
HEADERS += ut_qtcontacts_fetch.h
SOURCES += ut_qtcontacts_fetch.cpp
