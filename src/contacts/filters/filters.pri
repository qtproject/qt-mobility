INCLUDEPATH += filters

PUBLIC_HEADERS += \
    filters/qcontactchangelogfilter.h \
    filters/qcontactdetailfilter.h \
    filters/qcontactdetailrangefilter.h \
    filters/qcontactfilters.h \
    filters/qcontactlocalidfilter.h \
    filters/qcontactintersectionfilter.h \
    filters/qcontactinvalidfilter.h \
    filters/qcontactrelationshipfilter.h \
    filters/qcontactunionfilter.h

PRIVATE_HEADERS += \
    filters/qcontactchangelogfilter_p.h \
    filters/qcontactdetailfilter_p.h \
    filters/qcontactdetailrangefilter_p.h \
    filters/qcontactlocalidfilter_p.h \
    filters/qcontactintersectionfilter_p.h \
    filters/qcontactrelationshipfilter_p.h \
    filters/qcontactunionfilter_p.h

SOURCES += \
    filters/qcontactchangelogfilter.cpp \
    filters/qcontactdetailfilter.cpp \
    filters/qcontactdetailrangefilter.cpp \
    filters/qcontactlocalidfilter.cpp \
    filters/qcontactintersectionfilter.cpp \
    filters/qcontactinvalidfilter.cpp \
    filters/qcontactrelationshipfilter.cpp \
    filters/qcontactunionfilter.cpp
