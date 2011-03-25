TEMPLATE = app
TARGET = 
DEPENDPATH += .

CONFIG += link_pkgconfig
PKGCONFIG += qmfclient

macx:CONFIG-=app_bundle

QMF_INCLUDEDIR = $$(QMF_INCLUDEDIR)
QMF_LIBDIR = $$(QMF_LIBDIR)

# QMF headers must be located at $QMF_INCLUDEDIR
!isEmpty(QMF_INCLUDEDIR): INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
macx {
    !isEmpty(QMF_LIBDIR): LIBS += -F$$(QMF_LIBDIR)
    LIBS += -framework qmfclient
} else {
    !isEmpty(QMF_LIBDIR): LIBS += -L $$(QMF_LIBDIR)
    LIBS += -lqmfclient
}
!isEmpty(QMF_LIBDIR): QMAKE_RPATHDIR+= $$(QMF_LIBDIR)

# Input
SOURCES += main.cpp
