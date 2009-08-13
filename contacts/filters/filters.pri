INCLUDEPATH += filters

PUBLIC_HEADERS += \
    filters/qcontactactionfilter.h \
    filters/qcontactchangelogfilter.h \
    filters/qcontactdetailfilter.h \
    filters/qcontactdetailrangefilter.h \
    filters/qcontactfilters.h \
    filters/qcontactintersectionfilter.h \
    filters/qcontactunionfilter.h \
    filters/qcontactinvalidfilter.h \
    filters/qcontactidlistfilter.h

PRIVATE_HEADERS += \
    filters/qcontactactionfilter_p.h \
    filters/qcontactchangelogfilter_p.h \
    filters/qcontactdetailfilter_p.h \
    filters/qcontactdetailrangefilter_p.h \
    filters/qcontactgroupmembershipfilter.h \
    filters/qcontactgroupmembershipfilter_p.h \
    filters/qcontactintersectionfilter_p.h \
    filters/qcontactunionfilter_p.h \
    filters/qcontactidlistfilter_p.h

SOURCES += \
    filters/qcontactactionfilter.cpp \
    filters/qcontactchangelogfilter.cpp \
    filters/qcontactdetailfilter.cpp \
    filters/qcontactdetailrangefilter.cpp \
    filters/qcontactgroupmembershipfilter.cpp \
    filters/qcontactintersectionfilter.cpp \
    filters/qcontactunionfilter.cpp \
    filters/qcontactinvalidfilter.cpp \
    filters/qcontactidlistfilter.cpp
