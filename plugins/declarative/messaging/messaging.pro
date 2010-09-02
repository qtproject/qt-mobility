INCLUDEPATH += ../../../src/messaging
INCLUDEPATH += ../../../src/global

TARGET  = $$qtLibraryTarget(declarative_messaging)
TEMPLATE = lib
CONFIG += plugin
TARGETPATH = QtMobility/messaging
PLUGIN_TYPE = declarative

include(../../../common.pri)

QT += declarative

SOURCES += \
    qdeclarativemessagefilter.cpp \
    qdeclarativemessagemodel.cpp \
    plugin.cpp

HEADERS += \
    qdeclarativemessagefilter.h \
    qdeclarativemessagemodel.h


CONFIG += mobility
MOBILITY += messaging

DESTDIR = $$[QT_INSTALL_PREFIX]/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

INSTALLS += target qmldir

# QMF libraries must be located at $QMF_LIBDIR
simulator|contains(qmf_enabled, yes) {
    mac {
        QMAKE_LFLAGS += -F$$(QMF_LIBDIR)
        LIBS += -framework qtopiamail
    } else {
        LIBS += -L$$(QMF_LIBDIR) -l$$qtLibraryTarget(qtopiamail)
    }
}

