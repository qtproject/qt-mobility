TEMPLATE = app
TARGET = 
DEPENDPATH += .

macx:CONFIG-=app_bundle

# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
macx {
    LIBS += -F$$(QMF_LIBDIR) -framework qmfclient
} else {
    LIBS += -L $$(QMF_LIBDIR) -lqmfclient
}
QMAKE_RPATHDIR+= $$(QMF_LIBDIR)

# Input
SOURCES += main.cpp
