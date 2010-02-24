message("qtcontacts-tracker.pri")
!contains(DEFINES, QTCONTACTS_TRACKER_PRI) {
message(" ^ including")
DEFINES += QTCONTACTS_TRACKER_PRI


INCLUDEPATH += /usr/include/qt4/QtMobility
INCLUDEPATH += $$PWD

LIBS += -lqttracker
LIBS += -lQtContacts

HEADERS += $$PWD/qcontacttrackerbackend_p.h \
           $$PWD/qtrackercontactasyncrequest.h \
           $$PWD/trackerchangelistener.h \
           $$PWD/qtrackercontactslive.h \
           $$PWD/qtrackercontactsaverequest.h \
           $$PWD/qtrackerrelationshipfetchrequest.h \
           $$PWD/qtrackerrelationshipsaverequest.h \
           $$PWD/qtrackercontactidfetchrequest.h 

           

SOURCES += $$PWD/qcontacttrackerbackend.cpp \
           $$PWD/qtrackercontactasyncrequest.cpp \
           $$PWD/trackerchangelistener.cpp \
           $$PWD/qtrackercontactslive.cpp \
           $$PWD/qtrackercontactsaverequest.cpp \
           $$PWD/qtrackerrelationshipfetchrequest.cpp \
           $$PWD/qtrackerrelationshipsaverequest.cpp \
           $$PWD/qtrackercontactidfetchrequest.cpp 
           
}
