TEMPLATE=app
TARGET=tst_qmlmapsandnav
CONFIG += warn_on testcase
SOURCES += tst_qmlmapsandnav.cpp
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"

# Stuff below will likely be abstracted to single .pri -file.
#include(../../../../../qmltest/qtest-qml/src/quicktestlib/quicktestlib_dep.pri)

QT += declarative

INCLUDEPATH += $$PWD
LIBS += -L../../lib -L../../bin

win32:CONFIG(debug, debug|release) {
    LIBS += -lQtQuickTest$${QT_LIBINFIX}d
} else {
    LIBS += -lQtQuickTest$${QT_LIBINFIX}
}

# Locate the "lib" directory in the build tree and put it before
# the Qt "lib" directory in the library path so that we link
# against the libQtQuickTest.so in our build tree, not the Qt one.

# The original line, tweak according to  your installation.:
#FIND_TOP=../
FIND_TOP=../../../qmltest/qtest-qml/


for(i,forever) {
    exists($$_PRO_FILE_PWD_/$$FIND_TOP/qtest-qml.pro):break()
    FIND_TOP=../$$FIND_TOP
}
load(qt)
QMAKE_LIBDIR=$$OUT_PWD/$$FIND_TOP/lib $$QMAKE_LIBDIR
