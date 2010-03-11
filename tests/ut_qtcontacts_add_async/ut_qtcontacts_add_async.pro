TARGET = ut_qtcontacts_add_async

test.depends = all
QMAKE_EXTRA_TARGETS += test
QCONTACTS_TRACKER_BACKENDDIR = ../../

CONFIG += test
QT += testlib

LIBS += -lqttracker
LIBS += -lQtContacts

MOC_DIR = .moc
OBJECTS_DIR = .obj

CONFIG += mobility
MOBILITY += contacts 

INCLUDEPATH += /usr/include/qt4/QtContacts \
               /usr/include \
               $$QCONTACTS_TRACKER_BACKENDDIR

DEFINES += VERSION_INFO=\\\"0\\\"

## Include source files under test.
HEADERS += $$QCONTACTS_TRACKER_BACKENDDIR/qcontacttrackerbackend_p.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactasyncrequest.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/trackerchangelistener.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactslive.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactsaverequest.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackerrelationshipfetchrequest.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackerrelationshipsaverequest.h \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactidfetchrequest.h


SOURCES += $$QCONTACTS_TRACKER_BACKENDDIR/qcontacttrackerbackend.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactasyncrequest.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/trackerchangelistener.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactslive.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactsaverequest.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackerrelationshipfetchrequest.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackerrelationshipsaverequest.cpp \
           $$QCONTACTS_TRACKER_BACKENDDIR/qtrackercontactidfetchrequest.cpp


## Include unit test files
HEADERS += ut_qtcontacts_add_async.h

SOURCES += ut_qtcontacts_add_async.cpp
