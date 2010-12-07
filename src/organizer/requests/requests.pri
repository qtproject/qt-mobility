INCLUDEPATH += requests \
    ./

PUBLIC_HEADERS += requests/qorganizeritemrequests.h \
    requests/qorganizercollectionfetchrequest.h \
    requests/qorganizercollectionremoverequest.h \
    requests/qorganizercollectionsaverequest.h \
    requests/qorganizeritemdetaildefinitionfetchrequest.h \
    requests/qorganizeritemdetaildefinitionremoverequest.h \
    requests/qorganizeritemdetaildefinitionsaverequest.h \
    requests/qorganizeritemfetchrequest.h \
    requests/qorganizeritemfetchforexportrequest.h \
    requests/qorganizeritemfetchbyidrequest.h \
    requests/qorganizeritemoccurrencefetchrequest.h \
    requests/qorganizeritemidfetchrequest.h \
    requests/qorganizeritemremoverequest.h \
    requests/qorganizeritemsaverequest.h

PRIVATE_HEADERS += requests/qorganizeritemrequests_p.h

SOURCES += \
    requests/qorganizercollectionfetchrequest.cpp \
    requests/qorganizercollectionremoverequest.cpp \
    requests/qorganizercollectionsaverequest.cpp \
    requests/qorganizeritemdetaildefinitionfetchrequest.cpp \
    requests/qorganizeritemdetaildefinitionremoverequest.cpp \
    requests/qorganizeritemdetaildefinitionsaverequest.cpp \
    requests/qorganizeritemfetchrequest.cpp \
    requests/qorganizeritemfetchforexportrequest.cpp \
    requests/qorganizeritemfetchbyidrequest.cpp \
    requests/qorganizeritemoccurrencefetchrequest.cpp \
    requests/qorganizeritemidfetchrequest.cpp \
    requests/qorganizeritemremoverequest.cpp \
    requests/qorganizeritemsaverequest.cpp
