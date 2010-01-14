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
        ../../inc/cnttransformcontact.h \
        ../../inc/cnttransformcontactdata.h \
        ../../inc/cnttransformname.h \
        ../../inc/cnttransformnickname.h \
        ../../inc/cnttransformphonenumber.h \
        ../../inc/cnttransformemail.h \
        ../../inc/cnttransformaddress.h \
        ../../inc/cnttransformurl.h \
        ../../inc/cnttransformbirthday.h \
        ../../inc/cnttransformonlineaccount.h \
        ../../inc/cnttransformorganisation.h \
        ../../inc/cnttransformavatar.h \
        ../../inc/cnttransformavatarsimple.h \
        ../../inc/cntthumbnailcreator.h \        
        ../../inc/cnttransformsynctarget.h \
        ../../inc/cnttransformgender.h \
        ../../inc/cnttransformanniversary.h \
        ../../inc/cnttransformanniversarysimple.h \
        ../../inc/cnttransformgeolocation.h \
        ../../inc/cnttransformnote.h \
        ../../inc/cnttransformfamily.h \
        ../../inc/cnttransformempty.h \
        ../../inc/cntabstractcontactfilter.h \
        ../../inc/cntsymbianfilterdbms.h \
        ../../inc/cntsymbianfiltersql.h \
        ../../inc/cntsymbiansorterdbms.h \
        ../../inc/cntabstractcontactsorter.h \
        ../../inc/cntrelationship.h \
        ../../inc/cntabstractrelationship.h \
        ../../inc/cntrelationshipgroup.h \
        ../../inc/cntsymbianfiltersqlhelper.h \
        ../../inc/cntsymbiansrvconnection.h \
        ../../inc/cntsymbiantransformerror.h \
        ../../inc/cntsymbiandatabase.h \
        ../../inc/cntdisplaylabel.h \
        ../../inc/cntdisplaylabelsqlfilter.h 

SYMBIAN_SOURCES += \
        ../../src/cntsymbianengine.cpp \
        ../../src/cnttransformcontact.cpp \
        ../../src/cnttransformcontactdata.cpp \
        ../../src/cnttransformname.cpp \
        ../../src/cnttransformnickname.cpp \
        ../../src/cnttransformphonenumber.cpp \
        ../../src/cnttransformemail.cpp \
        ../../src/cnttransformaddress.cpp \
        ../../src/cnttransformurl.cpp \
        ../../src/cnttransformbirthday.cpp \
        ../../src/cnttransformonlineaccount.cpp \
        ../../src/cnttransformorganisation.cpp \
        ../../src/cnttransformavatar.cpp \
        ../../src/cnttransformavatarsimple.cpp \
        ../../src/cntthumbnailcreator.cpp \
        ../../src/cnttransformsynctarget.cpp \
        ../../src/cnttransformgender.cpp \
        ../../src/cnttransformanniversarysimple.cpp \
        ../../src/cnttransformanniversary.cpp \
        ../../src/cnttransformgeolocation.cpp \
        ../../src/cnttransformnote.cpp \
        ../../src/cnttransformfamily.cpp \
        ../../src/cnttransformempty.cpp \
        ../../src/cntsymbianfilterdbms.cpp \
        ../../src/cntsymbiansorterdbms.cpp \
        ../../src/cntsymbianfiltersql.cpp \
        ../../src/cntrelationship.cpp \
        ../../src/cntabstractrelationship.cpp \
        ../../src/cntrelationshipgroup.cpp \
        ../../src/cntsymbianfiltersqlhelper.cpp \
        ../../src/cntsymbiansrvconnection.cpp \
        ../../src/cntsymbiantransformerror.cpp \
        ../../src/cntsymbiandatabase.cpp \
        ../../src/cntdisplaylabel.cpp \
        ../../src/cntdisplaylabelsqlfilter.cpp 
 
exists($${EPOCROOT}epoc32/data/z/system/install/Series60v5.2.sis) {
        DEFINES += SYMBIAN_BACKEND_USE_SQLITE
    } 
 
SYMBIAN_LIBS += \
        -lcntmodel \
        -lQtContacts \
        -lcentralrepository \
        -lestor \
        -lefsrv \
        -lfbscli \
        -limageconversion \
        -lbitmaptransforms \ 
        -lbafl
        
