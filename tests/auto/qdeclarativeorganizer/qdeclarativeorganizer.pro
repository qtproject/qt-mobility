load(qttest_p4)
include (../../../common.pri)
CONFIG += mobility
MOBILITY += organizer versit
QT += declarative

INCLUDEPATH += ../../../src/organizer
INCLUDEPATH += ../../../src/organizer/items
INCLUDEPATH += ../../../src/organizer/details
INCLUDEPATH += ../../../src/organizer/filters
INCLUDEPATH += ../../../src/organizer/requests
INCLUDEPATH += ../../../src/versit
INCLUDEPATH += ../../../src/versitorganizer
DEPENDPATH += ../../src/organizer
DEPENDPATH += ../../src/versit


INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/organizer

HEADERS += \
	 ../../../plugins/declarative/organizer/qdeclarativeorganizeritem_p.h \
         ../../../plugins/declarative/organizer/qdeclarativeorganizercollection_p.h \
	 ../../../plugins/declarative/organizer/qdeclarativeorganizeritemsortorder_p.h \
         ../../../plugins/declarative/organizer/qdeclarativeorganizeritemdetail_p.h \
	 ../../../plugins/declarative/organizer/qdeclarativeorganizermodel_p.h \
         ../../../plugins/declarative/organizer/qdeclarativeorganizeritemfetchhint_p.h \
	 ../../../plugins/declarative/organizer/qdeclarativeorganizerrecurrencerule_p.h \
         ../../../plugins/declarative/organizer/qdeclarativeorganizeritemfilter_p.h \
         ../../../plugins/declarative/organizer/qdeclarativeorganizeritemmetaobject_p.h


SOURCES += tst_qdeclarativeorganizer.cpp \
       ../../../plugins/declarative/organizer/qdeclarativeorganizeritemdetail.cpp   \
       ../../../plugins/declarative/organizer/qdeclarativeorganizermodel.cpp \
       ../../../plugins/declarative/organizer/qdeclarativeorganizeritemfilter.cpp  \
       ../../../plugins/declarative/organizer/qdeclarativeorganizeritem.cpp \
       ../../../plugins/declarative/organizer/qdeclarativeorganizeritemmetaobject.cpp

include(../../../plugins/declarative/common/dynamicproperties.pri)

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

