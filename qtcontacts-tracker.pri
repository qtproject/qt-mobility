message("qtcontacts-tracker.pri")
!contains(DEFINES, QTCONTACTS_TRACKER_PRI) {
message(" ^ including")
DEFINES += QTCONTACTS_TRACKER_PRI

include(../../common/debuglevel.pri)

INCLUDEPATH += /usr/include/qt4/QtContacts
INCLUDEPATH += $$PWD

LIBS += -lqttracker
LIBS += -lQtContacts

HEADERS += $$PWD/qcontacttrackerbackend_p.h \
           $$PWD/qtrackercontactasyncrequest.h \
           $$PWD/trackerchangelistener.h \
           $$PWD/qtrackercontactslive.h \
           $$PWD/qtrackercontactsaverequest.h

SOURCES += $$PWD/qcontacttrackerbackend.cpp \
           $$PWD/qtrackercontactasyncrequest.cpp \
           $$PWD/trackerchangelistener.cpp \
           $$PWD/qtrackercontactslive.cpp \
           $$PWD/qtrackercontactsaverequest.cpp
           
}
