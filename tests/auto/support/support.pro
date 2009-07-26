TEMPLATE = lib
TARGET = support

HEADERS += \
    support.h

symbian|win32 {
symbian {
SOURCES += \
    support_symbian.cpp
}
win32 {
SOURCES += \
    support_win.cpp
}
} else {
# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

# We need qmfhelpers from the library source
INCLUDEPATH += ../../../messaging

DEFINES += QMESSAGING_OPTIONAL QMESSAGING_OPTIONAL_FOLDER

SOURCES += \
    support_qmf.cpp
}

