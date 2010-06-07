INCLUDEPATH += filters \
    ./

PUBLIC_HEADERS += filters/qorganizeritemfilters.h \
    filters/qorganizeritemchangelogfilter.h \
    filters/qorganizeritemdetailfilter.h \
    filters/qorganizeritemdetailrangefilter.h \
    filters/qorganizeritemintersectionfilter.h \
    filters/qorganizeriteminvalidfilter.h \
    filters/qorganizeritemlocalidfilter.h \
    filters/qorganizeritemunionfilter.h \
    filters/qorganizeritemdatetimeperiodfilter.h
PRIVATE_HEADERS += \
    filters/qorganizeritemchangelogfilter_p.h \
    filters/qorganizeritemdetailfilter_p.h \
    filters/qorganizeritemdetailrangefilter_p.h \
    filters/qorganizeritemintersectionfilter_p.h \
    filters/qorganizeritemlocalidfilter_p.h \
    filters/qorganizeritemunionfilter_p.h \
    filters/qorganizeritemdatetimeperiodfilter_p.h
SOURCES += \
    filters/qorganizeritemchangelogfilter.cpp \
    filters/qorganizeritemdetailfilter.cpp \
    filters/qorganizeritemdetailrangefilter.cpp \
    filters/qorganizeritemintersectionfilter.cpp \
    filters/qorganizeriteminvalidfilter.cpp \
    filters/qorganizeritemlocalidfilter.cpp \
    filters/qorganizeritemunionfilter.cpp \
    filters/qorganizeritemdatetimeperiodfilter.cpp
