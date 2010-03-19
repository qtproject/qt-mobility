CONFIG += mobility test
MOBILITY += contacts
QT += testlib

MOC_DIR = .moc
OBJECTS_DIR = .obj

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD
LIBS += -lqttracker

HEADERS += $$PWD/ut_qtcontacts_common.h
SOURCES += $$PWD/ut_qtcontacts_common.cpp
