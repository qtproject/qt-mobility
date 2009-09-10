SOURCES  += networkmanagertest.cpp nmview.cpp
HEADERS  += nmview.h
TARGET = tst_networkmanagertest

QT = core network dbus gui

#not really a test case but deployment happens same way
CONFIG += testcase

requires(contains(QT_CONFIG,dbus))

INCLUDEPATH += ../../bearer
include(../../common.pri)

LIBS += -L../../build/Debug/bin -lQtBearer

#MOC_DIR = .moc
#OBJECTS_DIR = .obj
FORMS += dialog.ui
