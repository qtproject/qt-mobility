TEMPLATE = lib
TARGET = QtMessaging

include(../../common.pri)

INCLUDEPATH += .

#Input

DEFINES += QT_BUILD_MESSAGING_LIB QT_MAKEDLL

PUBLIC_HEADERS += \
           qmessageid.h \
           qmessagecontentcontainerid.h \
           qmessagefolderid.h \
           qmessageaccountid.h \
           qmessagecontentcontainer.h \ 
           qmessagecontentcontainer_p.h \
           addresshelper_p.h \
	   qmessageaddress.h \
           qmessageaddress_p.h \
           qmessage_p.h \ 
           qmessage.h \
           qmessagefolder.h \
           qmessageaccount.h \
           qmessageaccountfilter.h \
           qmessageaccountsortorder.h \
           qmessagefolderfilter.h \
           qmessagefoldersortorder.h \
           qmessagefilter.h \
           qmessagemanager.h \
           qmessagesortorder.h \
           qmessageservice.h

PRIVATE_HEADERS += \
           qmessageid_p.h \
           qmessagecontentcontainerid_p.h \
           qmessagefolderid_p.h \
           qmessageaccountid_p.h \
           qmessagecontentcontainer_p.h \
           qmessageaddress_p.h \
           qmessage_p.h \ 
           qmessagefolder_p.h \ 
           qmessageaccount_p.h \
           qmessageaccountfilter_p.h \
           qmessageaccountsortorder_p.h \
           qmessagefolderfilter_p.h \
           qmessagefoldersortorder_p.h \
           qmessagefilter_p.h \
           qmessagesortorder_p.h \
           qmessagestore.h \
           qmessagestore_p.h

SOURCES += qmessageid.cpp \
           qmessagecontentcontainerid.cpp \
           qmessagefolderid.cpp \
           qmessageaccountid.cpp \
           qmessagecontentcontainer.cpp \
           addresshelper.cpp \
	   qmessageaddress.cpp \
           qmessage.cpp \
           qmessagefolder.cpp \
           qmessageaccount.cpp \
           qmessageaccountfilter.cpp \
           qmessageaccountsortorder.cpp \
           qmessagefolderfilter.cpp \
           qmessagefoldersortorder.cpp \
           qmessagefilter.cpp \
           qmessagemanager.cpp \
           qmessagesortorder.cpp \
           qmessagestore.cpp \
           qmessageservice.cpp


symbian|win32|maemo6 {
maemo6 {
SOURCES += qmessageid_maemo.cpp \
           qmessagecontentcontainerid_maemo.cpp \
           qmessagefolderid_maemo.cpp \
           qmessageaccountid_maemo.cpp \
           qmessagecontentcontainer_maemo.cpp \
           qmessage_maemo.cpp \
           qmessagefolder_maemo.cpp \
           qmessageaccount_maemo.cpp \
           qmessageaccountfilter_maemo.cpp \
           qmessageaccountsortorder_maemo.cpp \
           qmessagefolderfilter_maemo.cpp \
           qmessagefoldersortorder_maemo.cpp \
           qmessagefilter_maemo.cpp \
           qmessagesortorder_maemo.cpp \
           qmessagestore_maemo.cpp \
           qmessageservice_maemo.cpp 
}
symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    HEADERS -= qmessagestore_p.h \
               qmessagecontentcontainer_p.h \
               qmessage_p.h

    HEADERS += qmtmengine_symbian_p.h \
               qmessagestore_symbian_p.h \
               qmessageservice_symbian_p.h \
               qmessagecontentcontainer_symbian_p.h \
               qmessage_symbian_p.h

    SOURCES += qmtmengine_symbian.cpp \
               qmessageid_symbian.cpp \
               qmessagecontentcontainerid_symbian.cpp \
               qmessagefolderid_symbian.cpp \
               qmessageaccountid_symbian.cpp \
               qmessagecontentcontainer_symbian.cpp \
               qmessage_symbian.cpp \
               qmessagefolder_symbian.cpp \
               qmessageaccount_symbian.cpp \
               qmessageaccountfilter_symbian.cpp \
               qmessageaccountsortorder_symbian.cpp \
               qmessagefolderfilter_symbian.cpp \
               qmessagefoldersortorder_symbian.cpp \
               qmessagefilter_symbian.cpp \
               qmessagesortorder_symbian.cpp \
               qmessagestore_symbian.cpp \
               qmessageservice_symbian.cpp
           
    LIBS += -lsendas2 \
            -lmsgs \
            -letext \
            -lefsrv \
            -lcharconv \
            -lgsmu \
            -limcm \
            -lbafl \
            -lmtur \
            -lsendui \
    	    -lsmcm \
            -limcm \
            -leikcore \
            -lcone \
            -lapgrfx \
            -lapmime

    TARGET.CAPABILITY = ALL -TCB
    TARGET.UID3 = 0x2002AC82

    deploy.path = $${EPOCROOT}
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }
            
    QtMessaging.sources = QtMessaging.dll
    QtMessaging.path = /sys/bin
    DEPLOYMENT += QtMessaging
}

win32 {
PRIVATE_HEADERS += \
           winhelpers_p.h

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
           qmessageaccountsortorder_win.cpp \
           qmessagefolderfilter_win.cpp \
           qmessagefoldersortorder_win.cpp \
           qmessagefilter_win.cpp \
           qmessagesortorder_win.cpp \
           qmessagestore_win.cpp \
           qmessageservice_win.cpp 

wince* {
    # Include the source files from QMF needed for MIME parsing
    # These files are copied directly from the QMF repo with no changes:

    PRIVATE_HEADERS += win32wce/qmailaddress.h \
                       win32wce/qmailcodec.h \
                       win32wce/qmailfolderfwd.h \
                       win32wce/qmailglobal.h \
                       win32wce/qmailid.h \
                       win32wce/qmailipc.h \
                       win32wce/qmaillog.h \
                       win32wce/qmailmessage.h \
                       win32wce/qmailmessagefwd.h \
                       win32wce/qmailmessage_p.h \
                       win32wce/qmailnamespace.h \
                       win32wce/qmailtimestamp.h \
                       win32wce/longstring_p.h \
                       win32wce/qprivateimplementation.h \
                       win32wce/qprivateimplementationdef.h

    SOURCES += win32wce/qmailaddress.cpp \
               win32wce/qmailcodec.cpp \
               win32wce/qmailid.cpp \
               win32wce/qmailinstantiations.cpp \
               win32wce/qmaillog.cpp \
               win32wce/qmailmessage.cpp \
               win32wce/qmailmessagefwd.cpp \
               win32wce/qmailnamespace.cpp \
               win32wce/qmailtimestamp.cpp \
               win32wce/longstring.cpp \
               win32wce/qprivateimplementation.cpp

    DEFINES += QTOPIAMAIL_PARSING_ONLY QTOPIAMAIL_OMIT_QCOP SINGLE_MODULE_QTOPIAMAIL
    
    LIBS += cemapi.lib strmiids.lib uuid.lib
}
else {
    LIBS += mapi32.lib shlwapi.lib user32.lib
}

}
} else {
DEFINES += USE_QMF_IMPLEMENTATION

# QMF headers must be located at $QMF_INCLUDEDIR
INCLUDEPATH += $$(QMF_INCLUDEDIR) $$(QMF_INCLUDEDIR)/support

# QMF libraries must be located at $QMF_LIBDIR
LIBS += -L $$(QMF_LIBDIR) -lqtopiamail

PRIVATE_HEADERS += \
           qmfhelpers_p.h \
           qmessagestore_qmf_p.h

SOURCES += qmessageid_qmf.cpp \
           qmessagecontentcontainerid_qmf.cpp \
           qmessagefolderid_qmf.cpp \
           qmessageaccountid_qmf.cpp \
           qmessagecontentcontainer_qmf.cpp \
           qmessage_qmf.cpp \
           qmessagefolder_qmf.cpp \
           qmessageaccount_qmf.cpp \
           qmessageaccountfilter_qmf.cpp \
           qmessageaccountsortorder_qmf.cpp \
           qmessagefolderfilter_qmf.cpp \
           qmessagefoldersortorder_qmf.cpp \
           qmessagefilter_qmf.cpp \
           qmessagesortorder_qmf.cpp \
           qmessagestore_qmf.cpp \
           qmessageservice_qmf.cpp \
           qmfhelpers.cpp
}

HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS

include(../../features/deploy.pri)
