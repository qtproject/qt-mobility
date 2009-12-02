!contains(DEFINES, QTCONTACTS_TRACKER_PRI) {
DEFINES += QTCONTACTS_TRACKER_PRI

INCLUDEPATH += $$SOURCE_DIR/contacts
INCLUDEPATH += $$SOURCE_DIR/contacts/details
INCLUDEPATH += $$SOURCE_DIR/contacts/filters
INCLUDEPATH += $$SOURCE_DIR/contacts/requests

INCLUDEPATH += $$PWD

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
