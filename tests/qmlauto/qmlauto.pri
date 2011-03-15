CONFIG += warn_on testcase
DEFINES += QUICK_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
CONFIG += mobility

QT += declarative testlib
INCLUDEPATH += $$PWD

win32:CONFIG(debug, debug|release) {
    LIBS += -lQtQuickTest$${QT_LIBINFIX}d
} else {
    LIBS += -lQtQuickTest$${QT_LIBINFIX}
}


include(../../common.pri)


