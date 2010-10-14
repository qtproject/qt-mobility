INCLUDEPATH += filters \
    ./

PUBLIC_HEADERS += filters/qorganizeritemfilters.h \
    filters/qorganizeritemchangelogfilter.h \
    filters/qorganizeritemdetailfilter.h \
    filters/qorganizeritemdetailrangefilter.h \
    filters/qorganizeritemintersectionfilter.h \
    filters/qorganizeriteminvalidfilter.h \
    filters/qorganizeritemidfilter.h \
    filters/qorganizeritemcollectionfilter.h \
    filters/qorganizeritemunionfilter.h
PRIVATE_HEADERS += \
    filters/qorganizeritemchangelogfilter_p.h \
    filters/qorganizeritemdetailfilter_p.h \
    filters/qorganizeritemdetailrangefilter_p.h \
    filters/qorganizeritemintersectionfilter_p.h \
    filters/qorganizeritemidfilter_p.h \
    filters/qorganizeritemcollectionfilter_p.h \
    filters/qorganizeritemunionfilter_p.h
SOURCES += \
    filters/qorganizeritemchangelogfilter.cpp \
    filters/qorganizeritemdetailfilter.cpp \
    filters/qorganizeritemdetailrangefilter.cpp \
    filters/qorganizeritemintersectionfilter.cpp \
    filters/qorganizeriteminvalidfilter.cpp \
    filters/qorganizeritemidfilter.cpp \
    filters/qorganizeritemcollectionfilter.cpp \
    filters/qorganizeritemunionfilter.cpp
