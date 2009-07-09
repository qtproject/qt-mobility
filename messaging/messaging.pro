TEMPLATE = lib

TARGET = QtMessaging

include(../common.pri)

#Input

HEADERS += draft-api.h \
           qmessageid.h \
           qmessageid_p.h \
           qmessagecontentcontainerid.h \
           qmessagecontentcontainerid_p.h \
           qmessagefolderid.h \
           qmessagefolderid_p.h \
           qmessageaccountid.h \
           qmessageaccountid_p.h \
           qmessagecontentcontainer.h \ 
           qmessagecontentcontainer_p.h \
           qmessageaddress.h \ 
           qmessageaddress_p.h \
           qmessage_p.h \ 
           qmessage.h \
           qmessagefolder_p.h \ 
           qmessagefolder.h \
           qmessageaccountfilterkey \
           qmessageaccountfilterkey_p.h \
           qmessageaccountsortkey \
           qmessageaccountsortkey_p.h \
           qmessagefolderfilterkey \
           qmessagefolderfilterkey_p.h \
           qmessagefoldersortkey \
           qmessagefoldersortkey_p.h \
           qmessagefilterkey \
           qmessagefilterkey_p.h \
           qmessagesortkey \
           qmessagesortkey_p.h 

SOURCES += draft-api.cpp \
           qmessageid.cpp \
           qmessagecontentcontainerid.cpp \
           qmessagefolderid.cpp \
           qmessageaccountid.cpp \
           qmessagecontentcontainer.cpp \
           qmessageaddress.cpp \
           qmessage.cpp \
           qmessagefolder.cpp \
           qmessageaccountfilterkey.cpp \
           qmessageaccountsortkey.cpp \
           qmessagefolderfilterkey.cpp \
           qmessagefoldersortkey.cpp \
           qmessagefilterkey.cpp \
           qmessagesortkey.cpp
