TEMPLATE = lib
TARGET = QtMessaging

include(../common.pri)

INCLUDEPATH += .

#Input

DEFINES += QT_BUILD_MESSAGING_LIB

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
           qmessageaccountfilter.h \
           qmessageaccountfilter_p.h \
           qmessageaccountordering.h \
           qmessageaccountordering_p.h \
           qmessagefolderfilter.h \
           qmessagefolderfilter_p.h \
           qmessagefolderordering.h \
           qmessagefolderordering_p.h \
           qmessagefilter.h \
           qmessagefilter_p.h \
           qmessageordering.h \
           qmessageordering_p.h \
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
           qmessageaccountfilter.cpp \
           qmessageaccountordering.cpp \
           qmessagefolderfilter.cpp \
           qmessagefolderordering.cpp \
           qmessagefilter.cpp \
           qmessageordering.cpp \
           qmessagestore.cpp \
           qmessageserviceaction.cpp


symbian|win32|maemo {
maemo {
SOURCES += qmessageid_maemo.cpp \
           qmessagecontentcontainerid_maemo.cpp \
           qmessagefolderid_maemo.cpp \
           qmessageaccountid_maemo.cpp \
           qmessagecontentcontainer_maemo.cpp \
           qmessage_maemo.cpp \
           qmessagefolder_maemo.cpp \
           qmessageaccount_maemo.cpp \
           qmessageaccountfilter_maemo.cpp \
           qmessageaccountordering_maemo.cpp \
           qmessagefolderfilter_maemo.cpp \
           qmessagefolderordering_maemo.cpp \
           qmessagefilter_maemo.cpp \
           qmessageordering_maemo.cpp \
           qmessagestore_maemo.cpp \
           qmessageserviceaction_maemo.cpp 
}
symbian {
SOURCES += qmessageid_symbian.cpp \
           qmessagecontentcontainerid_symbian.cpp \
           qmessagefolderid_symbian.cpp \
           qmessageaccountid_symbian.cpp \
           qmessagecontentcontainer_symbian.cpp \
           qmessage_symbian.cpp \
           qmessagefolder_symbian.cpp \
           qmessageaccount_symbian.cpp \
           qmessageaccountfilter_symbian.cpp \
           qmessageaccountordering_symbian.cpp \
           qmessagefolderfilter_symbian.cpp \
           qmessagefolderordering_symbian.cpp \
           qmessagefilter_symbian.cpp \
           qmessageordering_symbian.cpp \
           qmessagestore_symbian.cpp \
           qmessageserviceaction_symbian.cpp 
}
win32 {
!static:DEFINES += QT_MAKEDLL

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
           qmessageaccountfilter_win.cpp \
           qmessageaccountordering_win.cpp \
           qmessagefolderfilter_win.cpp \
           qmessagefolderordering_win.cpp \
           qmessagefilter_win.cpp \
           qmessageordering_win.cpp \
           qmessagestore_win.cpp \
           qmessageserviceaction_win.cpp 

LIBS += mapi32.lib shlwapi.lib
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
           qmessageaccountfilter_qmf.cpp \
           qmessageaccountordering_qmf.cpp \
           qmessagefolderfilter_qmf.cpp \
           qmessagefolderordering_qmf.cpp \
           qmessagefilter_qmf.cpp \
           qmessageordering_qmf.cpp \
           qmessagestore_qmf.cpp \
           qmessageserviceaction_qmf.cpp \
           qmfhelpers.cpp
}
