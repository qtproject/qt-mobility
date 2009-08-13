INCLUDEPATH += requests

PUBLIC_HEADERS += \
    requests/qcontactfetchrequest.h \
    requests/qcontactidfetchrequest.h \
    requests/qcontactsaverequest.h \
    requests/qcontactremoverequest.h \
    requests/qcontactgroupfetchrequest.h \
    requests/qcontactgroupsaverequest.h \
    requests/qcontactgroupremoverequest.h \
    requests/qcontactdetaildefinitionfetchrequest.h \
    requests/qcontactdetaildefinitionsaverequest.h \
    requests/qcontactdetaildefinitionremoverequest.h \
    requests/qcontactrequests.h

PRIVATE_HEADERS += \
    requests/qcontactrequests_p.h

SOURCES += \
    requests/qcontactfetchrequest.cpp \
    requests/qcontactidfetchrequest.cpp \
    requests/qcontactsaverequest.cpp \
    requests/qcontactremoverequest.cpp \
    requests/qcontactgroupfetchrequest.cpp \
    requests/qcontactgroupsaverequest.cpp \
    requests/qcontactgroupremoverequest.cpp \
    requests/qcontactdetaildefinitionfetchrequest.cpp \
    requests/qcontactdetaildefinitionsaverequest.cpp \
    requests/qcontactdetaildefinitionremoverequest.cpp
