message("qtcontacts-tracker.pri")

!contains(DEFINES, QTCONTACTS-TRACKER_PRI) {
message(" ^ including")
DEFINES += QTCONTACTS-TRACKER_PRI

INCLUDEPATH += /usr/include/qt4/QtContacts

LIBS += -lqttracker
LIBS += -lQtContacts

HEADERS += $$PWD/qcontacttrackerbackend_p.h \
           $$PWD/qtrackercontactasyncrequest.h \
           $$PWD/trackerchangelistener.h 
           

SOURCES += $$PWD/qcontacttrackerbackend.cpp \
           $$PWD/qtrackercontactasyncrequest.cpp \
           $$PWD/trackerchangelistener.cpp
}
