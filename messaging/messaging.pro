TEMPLATE = lib

TARGET = QtMessaging
INCLUDEPATH += .

include(../common.pri)

#Input

!static:DEFINES += QT_MAKEDLL
DEFINES += QT_BUILD_MESSAGING_LIB
DEFINES += QMESSAGING_OPTIONAL QMESSAGING_OPTIONAL_FOLDER

HEADERS += qmessageglobal.h \
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
           qmessageaccount_p.h \
           qmessageaccount.h \
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
           qmessageaccount.cpp \
           qmessageaccountfilterkey.cpp \
           qmessageaccountsortkey.cpp \
           qmessagefolderfilterkey.cpp \
           qmessagefoldersortkey.cpp \
           qmessagefilterkey.cpp \
           qmessagesortkey.cpp \
           qmessagestore.cpp \
           qmessageserviceaction.cpp


symbian|win32 {
symbian {
SOURCES += qmessageid_symbian.cpp \
           qmessagecontentcontainerid_symbian.cpp \
           qmessagefolderid_symbian.cpp \
           qmessageaccountid_symbian.cpp \
           qmessagecontentcontainer_symbian.cpp \
           qmessage_symbian.cpp \
           qmessagefolder_symbian.cpp \
           qmessageaccount_symbian.cpp \
           qmessageaccountfilterkey_symbian.cpp \
           qmessageaccountsortkey_symbian.cpp \
           qmessagefolderfilterkey_symbian.cpp \
           qmessagefoldersortkey_symbian.cpp \
           qmessagefilterkey_symbian.cpp \
           qmessagesortkey_symbian.cpp \
           qmessagestore_symbian.cpp \
           qmessageserviceaction_symbian.cpp 
}
win32 {
HEADERS += winhelpers_p.h

SOURCES += winhelpers.cpp \
           qmessageid_win.cpp \
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

LIBS += mapi32.lib 
}
} else {
# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

HEADERS += qmfhelpers_p.h

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
           qmessageserviceaction_qmf.cpp \
           qmfhelpers.cpp
}
