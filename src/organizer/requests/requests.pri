INCLUDEPATH += requests \
    ./

PUBLIC_HEADERS += requests/qorganizeritemrequests.h \
    requests/qorganizeritemdetaildefinitionfetchrequest.h \
    requests/qorganizeritemdetaildefinitionremoverequest.h \
    requests/qorganizeritemdetaildefinitionsaverequest.h \
    requests/qorganizeritemfetchrequest.h \
    requests/qorganizeriteminstancefetchrequest.h \
    requests/qorganizeritemlocalidfetchrequest.h \
    requests/qorganizeritemremoverequest.h \
    requests/qorganizeritemsaverequest.h

PRIVATE_HEADERS += requests/qorganizeritemrequests_p.h

SOURCES += \
    requests/qorganizeritemdetaildefinitionfetchrequest.cpp \
    requests/qorganizeritemdetaildefinitionremoverequest.cpp \
    requests/qorganizeritemdetaildefinitionsaverequest.cpp \
    requests/qorganizeritemfetchrequest.cpp \
    requests/qorganizeriteminstancefetchrequest.cpp \
    requests/qorganizeritemlocalidfetchrequest.cpp \
    requests/qorganizeritemremoverequest.cpp \
    requests/qorganizeritemsaverequest.cpp
