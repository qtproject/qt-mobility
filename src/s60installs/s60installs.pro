TEMPLATE = subdirs

symbian: {
    load(data_caging_paths)

    SUBDIRS =
    TARGET = "QtMobility preview"
    TARGET.UID3 = 0x2002AC89
    VERSION = 0.1.0

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    qtmobilitydeployment.pkg_prerules += vendorinfo

    EPOCROOT31 = $${EPOCROOT31}
    EPOCROOT50 = $${EPOCROOT50}
    
    qtmobilitydeployment.sources = \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtMessaging_tp.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtServiceFramework_tp.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/SFWDatabaseManagerServer.exe \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtLocation_tp.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtSystemInfo_tp.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtPublishSubscribe_tp.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtContacts_tp.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtVersit_tp.dll


    bearer = \
        "IF package(0x1028315F)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/QtBearer_tp.dll\" - \"!:\\sys\\bin\\QtBearer_tp.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/QtBearer_tp.dll\" - \"!:\\sys\\bin\\QtBearer_tp.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$$(EPOCROOT31)epoc32/release/armv5/urel/QtBearer_tp.dll\" - \"!:\\sys\\bin\\QtBearer_tp.dll\"" \
        "ELSE" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/QtBearer_tp.dll\" - \"!:\\sys\\bin\\QtBearer_tp.dll\"" \
        "ENDIF"

    contacts = \
        "IF package(0x1028315F)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$$(EPOCROOT31)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ELSE" \
        "   \"$$(EPOCROOT50)epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
        "ENDIF"

    pluginstubs = \
        "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbian/qmakepluginstubs/mobapicontactspluginsymbian.qtplugin\" - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbian.qtplugin\""

    qtmobilitydeployment.pkg_postrules += bearer
    qtmobilitydeployment.pkg_postrules += contacts
    qtmobilitydeployment.pkg_postrules += pluginstubs
    
    qtmobilitydeployment.path = /sys/bin
    
    DEPLOYMENT += qtmobilitydeployment 
}
