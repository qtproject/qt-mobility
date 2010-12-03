load(qttest_p4)
include (../../../common.pri)
CONFIG += mobility 
MOBILITY += contacts versit
QT += declarative

INCLUDEPATH += ../../../src/contacts
INCLUDEPATH += ../../../src/contacts/details
INCLUDEPATH += ../../../src/contacts/filters
INCLUDEPATH += ../../../src/contacts/requests
INCLUDEPATH += ../../../src/versit
DEPENDPATH += ../../src/contacts
DEPENDPATH += ../../src/versit


INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/contacts
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/contacts/details
INCLUDEPATH += $$QT_MOBILITY_SOURCE_TREE/plugins/declarative/contacts/filters

HEADERS += ../../../plugins/declarative/contacts/qdeclarativecontactdetail_p.h\
           ../../../plugins/declarative/contacts/qdeclarativeopenmetaobject_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactmetaobject_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactrelationship_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactfetchhint_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactmodel_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactsortorder_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactfilter_p.h\
 	   ../../../plugins/declarative/contacts/qdeclarativecontact_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactimageprovider_p.h\
           ../../../plugins/declarative/contacts/qdeclarativecontactrelationshipmodel_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactaddress_p.h\
	   ../../../plugins/declarative/contacts/details/qdeclarativecontactname_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactanniversary_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactnickname_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactavatar_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactnote_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactbirthday_p.h\
	   ../../../plugins/declarative/contacts/details/qdeclarativecontactonlineaccount_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactdetails_p.h\
	   ../../../plugins/declarative/contacts/details/qdeclarativecontactorganization_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactdisplaylabel_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactphonenumber_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactemailaddress_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactpresence_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactfamily_p.h\
	   ../../../plugins/declarative/contacts/details/qdeclarativecontactringtone_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactfavorite_p.h\
	   ../../../plugins/declarative/contacts/details/qdeclarativecontactsynctarget_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactgender_p.h\
	   ../../../plugins/declarative/contacts/details/qdeclarativecontacttag_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactgeolocation_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontacttimestamp_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactglobalpresence_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontactguid_p.h\
           ../../../plugins/declarative/contacts/details/qdeclarativecontacturl_p.h \
           ../../../plugins/declarative/contacts/details/qdeclarativecontacthobby_p.h \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactactionfilter_p.h\
      	   ../../../plugins/declarative/contacts/filters/qdeclarativecontactintersectionfilter_p.h \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactchangelogfilter_p.h	\
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactinvalidfilter_p.h \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactdetailfilter_p.h	\
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactlocalidfilter_p.h \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactdetailrangefilter_p.h \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactrelationshipfilter_p.h \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactfilters_p.h	 \
           ../../../plugins/declarative/contacts/filters/qdeclarativecontactunionfilter_p.h\
           ../../../plugins/declarative/contacts/qmetaobjectbuilder_p.h
SOURCES += tst_qdeclarativecontact.cpp \
           ../../../plugins/declarative/contacts/qdeclarativecontactdetail.cpp\
           ../../../plugins/declarative/contacts/qdeclarativeopenmetaobject.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactmetaobject.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactrelationship.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactfetchhint.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactmodel.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactsortorder.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactfilter.cpp\
 	   ../../../plugins/declarative/contacts/qdeclarativecontact.cpp\
           ../../../plugins/declarative/contacts/qdeclarativecontactrelationshipmodel.cpp\
           ../../../plugins/declarative/contacts/qmetaobjectbuilder.cpp
symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

