TEMPLATE = lib

TARGET = QtMessaging

include(../common.pri)

#Input

HEADERS += qmessageid.h \
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
           qmessageaccountfilterkey.h \
           qmessageaccountfilterkey_p.h \
           qmessageaccountsortkey.h \
           qmessageaccountsortkey_p.h \
           qmessagefolderfilterkey.h \
           qmessagefolderfilterkey_p.h \
           qmessagefoldersortkey.h \
           qmessagefoldersortkey_p.h \
           qmessagefilterkey.h \
           qmessagefilterkey_p.h \
           qmessagesortkey.h \
           qmessagesortkey_p.h \
           qmessagestore.h \
           qmessagestore_p.h \
           qmessageserviceaction.h \
           qmessageserviceaction_p.h

SOURCES += qmessageid.cpp \
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
           qmessagesortkey.cpp \
           qmessagestore.cpp \
           qmessageserviceaction.cpp

SOURCES += qmessageid_win.cpp \
           qmessagecontentcontainerid_win.cpp \
           qmessagefolderid_win.cpp \
           qmessageaccountid_win.cpp \
           qmessage_win.cpp \
           qmessagefolder_win.cpp \
           qmessageaccount_win.cpp \
           qmessagestore_win.cpp \
           qmessageserviceaction_win.cpp 
