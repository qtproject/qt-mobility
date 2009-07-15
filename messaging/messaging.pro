TEMPLATE = lib

TARGET = QtMessaging

include(../common.pri)

#Input

#DEFINES += QMESSAGING_OPTIONAL_FOLDER
#DEFINES += QMESSAGING_OPTIONAL

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

win32: {
SOURCES += qmessageid_win.cpp \
           qmessagecontentcontainerid_win.cpp \
           qmessagefolderid_win.cpp \
           qmessageaccountid_win.cpp \
           qmessagecontentcontainer_win.cpp \
           qmessage_win.cpp \
           qmessagefolder_win.cpp \
           qmessageaccount_win.cpp \
           qmessageaccountfilterkey_win.cpp \
           qmessageaccountsortkey_win.cpp \
           qmessagefolderfilterkey_win.cpp \
           qmessagefoldersortkey_win.cpp \
           qmessagefilterkey_win.cpp \
           qmessagesortkey_win.cpp \
           qmessagestore_win.cpp \
           qmessageserviceaction_win.cpp 
} else {
# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$QMF_INCLUDEDIR $$QMF_INCLUDEDIR/support

# QMF libraries must be located at $QMF_LIBDIR
LIBS += -L $$QMF_LIBDIR -lqtopiamail

DEFINES += QMESSAGING_OPTIONAL

SOURCES += qmessageid_qmf.cpp \
           qmessagecontentcontainerid_qmf.cpp \
           qmessagefolderid_qmf.cpp \
           qmessageaccountid_qmf.cpp \
           qmessagecontentcontainer_qmf.cpp \
           qmessage_qmf.cpp \
           qmessagefolder_qmf.cpp \
           qmessageaccount_qmf.cpp \
           qmessageaccountfilterkey_qmf.cpp \
           qmessageaccountsortkey_qmf.cpp \
           qmessagefolderfilterkey_qmf.cpp \
           qmessagefoldersortkey_qmf.cpp \
           qmessagefilterkey_qmf.cpp \
           qmessagesortkey_qmf.cpp \
           qmessagestore_qmf.cpp \
           qmessageserviceaction_qmf.cpp 
}
