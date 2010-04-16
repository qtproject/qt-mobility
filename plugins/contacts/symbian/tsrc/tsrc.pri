include(../symbian_defines.pri)

# Symbian plugin header and source files
SYMBIAN_PATHS += \
        ../../inc \
        ../../rss \
        ../../../../../src/global \
        ../../../../../src/contacts \
        ../../../../../src/contacts/details \
        ../../../../../src/contacts/filters \
        ../../../../../src/contacts/requests
        
SYMBIAN_HEADERS += \
        ../../inc/cntsymbianengine.h \
        ../../inc/transform/cnttransformcontact.h \
        ../../inc/transform/cnttransformcontactdata.h \
        ../../inc/transform/cnttransformname.h \
        ../../inc/transform/cnttransformnickname.h \
        ../../inc/transform/cnttransformphonenumber.h \
        ../../inc/transform/cnttransformemail.h \
        ../../inc/transform/cnttransformaddress.h \
        ../../inc/transform/cnttransformurl.h \
        ../../inc/transform/cnttransformbirthday.h \
        ../../inc/transform/cnttransformonlineaccount.h \
        ../../inc/transform/cnttransformpresence.h \
        ../../inc/transform/cnttransformorganisation.h \
        ../../inc/transform/cnttransformringtone.h \
        ../../inc/transform/cnttransformthumbnail.h \
        ../../inc/transform/cnttransformavatar.h \
        ../../inc/transform/cntthumbnailcreator.h \        
        ../../inc/transform/cnttransformsynctarget.h \
        ../../inc/transform/cnttransformgender.h \
        ../../inc/transform/cnttransformanniversary.h \
        ../../inc/transform/cnttransformanniversarysimple.h \
        ../../inc/transform/cnttransformgeolocation.h \
        ../../inc/transform/cnttransformnote.h \
        ../../inc/transform/cnttransformfamily.h \
        ../../inc/transform/cnttransformempty.h \
        ../../inc/filtering/cntabstractcontactfilter.h \
        ../../inc/filtering/cntsymbianfilterdbms.h \
        ../../inc/filtering/cntsymbianfiltersql.h \
        ../../inc/filtering/cntsymbiansorterdbms.h \
        ../../inc/filtering/cntabstractcontactsorter.h \
        ../../inc/cntrelationship.h \
        ../../inc/cntabstractrelationship.h \
        ../../inc/cntrelationshipgroup.h \
        ../../inc/filtering/cntsymbiansrvconnection.h \
        ../../inc/cntsymbiantransformerror.h \
        ../../inc/cntsymbiandatabase.h \
        ../../inc/cntdisplaylabel.h \
        ../../inc/filtering/cntdisplaylabelsqlfilter.h \
        ../../inc/filtering/cntsqlsearch.h \
        ../../inc/filtering/cntdbinfo.h \
        ../../inc/filtering/cntfilterdetail.h \
        ../../inc/filtering/cntfilterdefault.h \
        ../../inc/filtering/cntfilterintersection.h \
        ../../inc/filtering/cntfilterunion.h \
        ../../inc/filtering/cntfilterrelationship.h \
        ../../inc/filtering/cntfilterinvalid.h \
        ../../inc/filtering/cntfilterdetailrange.h \
        ../../inc/filtering/cntfilterchangelog.h \
        ../../inc/filtering/cntfilteraction.h \
        ../../inc/filtering/cntfilterlocalid.h \
        ../../inc/filtering/cntfilterdetaildisplaylabel.h \
        

SYMBIAN_SOURCES += \
        ../../src/cntsymbianengine.cpp \
        ../../src/transform/cnttransformcontact.cpp \
        ../../src/transform/cnttransformcontactdata.cpp \
        ../../src/transform/cnttransformname.cpp \
        ../../src/transform/cnttransformnickname.cpp \
        ../../src/transform/cnttransformphonenumber.cpp \
        ../../src/transform/cnttransformemail.cpp \
        ../../src/transform/cnttransformaddress.cpp \
        ../../src/transform/cnttransformurl.cpp \
        ../../src/transform/cnttransformbirthday.cpp \
        ../../src/transform/cnttransformonlineaccount.cpp \
        ../../src/transform/cnttransformpresence.cpp \
        ../../src/transform/cnttransformorganisation.cpp \
        ../../src/transform/cnttransformringtone.cpp \
        ../../src/transform/cnttransformthumbnail.cpp \        
        ../../src/transform/cnttransformavatar.cpp \
        ../../src/transform/cntthumbnailcreator.cpp \
        ../../src/transform/cnttransformsynctarget.cpp \
        ../../src/transform/cnttransformgender.cpp \
        ../../src/transform/cnttransformanniversarysimple.cpp \
        ../../src/transform/cnttransformanniversary.cpp \
        ../../src/transform/cnttransformgeolocation.cpp \
        ../../src/transform/cnttransformnote.cpp \
        ../../src/transform/cnttransformfamily.cpp \
        ../../src/transform/cnttransformempty.cpp \
        ../../src/filtering/cntsymbianfilterdbms.cpp \
        ../../src/filtering/cntsymbiansorterdbms.cpp \
        ../../src/filtering/cntsymbianfiltersql.cpp \
        ../../src/cntrelationship.cpp \
        ../../src/cntabstractrelationship.cpp \
        ../../src/cntrelationshipgroup.cpp \
        ../../src/filtering/cntsymbiansrvconnection.cpp \
        ../../src/cntsymbiantransformerror.cpp \
        ../../src/cntsymbiandatabase.cpp \
        ../../src/cntdisplaylabel.cpp \
        ../../src/filtering/cntdisplaylabelsqlfilter.cpp \
        ../../src/filtering/cntsqlsearch.cpp \
        ../../src/filtering/cntdbinfo.cpp \
        ../../src/filtering/cntfilterdetail.cpp \
        ../../src/filtering/cntfilterdefault.cpp \
        ../../src/filtering/cntfilterintersection.cpp \
        ../../src/filtering/cntfilterunion.cpp \
        ../../src/filtering/cntfilterrelationship.cpp \
        ../../src/filtering/cntfilterinvalid.cpp \
        ../../src/filtering/cntfilterdetailrange.cpp \
        ../../src/filtering/cntfilterchangelog.cpp \
        ../../src/filtering/cntfilteraction.cpp \
        ../../src/filtering/cntfilterlocalid.cpp \
        ../../src/filtering/cntfilterdetaildisplaylabel.cpp \
 
SYMBIAN_LIBS += \
        -lcntmodel \
        -lQtContacts \
        -lcentralrepository \
        -lestor \
        -lefsrv \
        -lfbscli \
        -limageconversion \
        -lbitmaptransforms \ 
        -lbafl \
        -lplatformenv
