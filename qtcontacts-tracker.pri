message("qtcontacts-tracker.pri")

!contains(DEFINES, QTCONTACTS-TRACKER_PRI) {
message(" ^ including")
DEFINES += QTCONTACTS-TRACKER_PRI


INCLUDEPATH += /usr/include/qt4/QtContacts
INCLUDEPATH += $$PWD

LIBS += -lqttracker
LIBS += -lQtContacts

HEADERS += qcontacttrackerbackend_p.h \
           qtrackercontactasyncrequest.h

SOURCES += qcontacttrackerbackend.cpp \
           qtrackercontactasyncrequest.cpp

}
