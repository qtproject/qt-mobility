INCLUDEPATH += requests

PUBLIC_HEADERS += \
    requests/qcontactfetchrequest.h \
    requests/qcontactlocalidfetchrequest.h \
    requests/qcontactsaverequest.h \
    requests/qcontactremoverequest.h \
    requests/qcontactdetaildefinitionfetchrequest.h \
    requests/qcontactdetaildefinitionsaverequest.h \
    requests/qcontactdetaildefinitionremoverequest.h \
    requests/qcontactrelationshipfetchrequest.h \
    requests/qcontactrelationshipremoverequest.h \
    requests/qcontactrelationshipsaverequest.h \
    requests/qcontactrequests.h

PRIVATE_HEADERS += \
    requests/qcontactrequests_p.h

SOURCES += \
    requests/qcontactfetchrequest.cpp \
    requests/qcontactlocalidfetchrequest.cpp \
    requests/qcontactsaverequest.cpp \
    requests/qcontactremoverequest.cpp \
    requests/qcontactdetaildefinitionfetchrequest.cpp \
    requests/qcontactdetaildefinitionsaverequest.cpp \
    requests/qcontactdetaildefinitionremoverequest.cpp \
    requests/qcontactrelationshipfetchrequest.cpp \
    requests/qcontactrelationshipremoverequest.cpp \
    requests/qcontactrelationshipsaverequest.cpp
