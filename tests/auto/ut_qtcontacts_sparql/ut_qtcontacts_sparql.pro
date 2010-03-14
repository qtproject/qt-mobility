TARGET = ut_qtcontacts_sparql

test.depends = all
QMAKE_EXTRA_TARGETS += test
QCONTACTS_TRACKER_BACKENDDIR = ../../

CONFIG += mobility test
MOBILITY += contacts
QT += testlib

LIBS += -lqttracker

MOC_DIR = .moc
OBJECTS_DIR = .obj

## Include unit test files
HEADERS += ut_qtcontacts_sparql.h

SOURCES += ut_qtcontacts_sparql.cpp
