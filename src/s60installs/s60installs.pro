TEMPLATE = subdirs

symbian: {
    include(../../staticconfig.pri)
    load(data_caging_paths)
    include($$QT_MOBILITY_BUILD_TREE/config.pri)

    SUBDIRS =
    TARGET = "QtMobility"
    TARGET.UID3 = 0x2002AC89
    # TP preview 0.1.0
    # Beta 0.2.0
    # Final 1.0.0

    VERSION = 1.0.0

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia\"" \
        " "
    qtmobilitydeployment.pkg_prerules += vendorinfo

    epoc31 = $$(EPOCROOT31)
    epoc32 = $$(EPOCROOT32)
    epoc50 = $$(EPOCROOT50)

    # default to EPOCROOT if EPOCROOTxy not defined
    isEmpty(epoc31) {
        EPOCROOT31 = $${EPOCROOT}
    } else {
    EPOCROOT31 = $$(EPOCROOT31)
    }
    isEmpty(epoc32) {
        EPOCROOT32 = $${EPOCROOT}
    }else {
    EPOCROOT32 = $$(EPOCROOT32)
    }
    isEmpty(epoc50) {
        EPOCROOT50 = $${EPOCROOT}
    } else {
    EPOCROOT50 = $$(EPOCROOT50)
    }

    contains(mobility_modules, messaging): qtmobilitydeployment.sources += \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtMessaging.dll

    contains(mobility_modules, serviceframework): qtmobilitydeployment.sources += \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtServiceFramework.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/SFWDatabaseManagerServer.exe

    contains(mobility_modules, location): qtmobilitydeployment.sources += \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtLocation.dll

    contains(mobility_modules, systeminfo): qtmobilitydeployment.sources += \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtSystemInfo.dll

    contains(mobility_modules, publishsubscribe): qtmobilitydeployment.sources += \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtPublishSubscribe.dll \
        $$(EPOCROOT50)epoc32/release/armv5/urel/PSPathMapperServer.exe

    contains(mobility_modules, versit): qtmobilitydeployment.sources += \
        $$(EPOCROOT50)epoc32/release/armv5/urel/QtVersit.dll 


    contains(mobility_modules, bearer) {
        bearer = \
            "IF package(0x1028315F)" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$$EPOCROOT31\epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ELSE" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += bearer
    }

    contains(mobility_modules, contacts) {

        qtmobilitydeployment.sources += \
            $$(EPOCROOT50)epoc32/release/armv5/urel/QtContacts.dll

        contacts = \
            "IF package(0x1028315F)" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$$EPOCROOT32\epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$$EPOCROOT31\epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ELSE" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ENDIF" \
            "\"$$EPOCROOT50\epoc32/release/armv5/urel/mobapicontactspluginsymbiansim.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbiansim.dll\""

        qtmobilitydeployment.pkg_postrules += contacts

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbian/qmakepluginstubs/mobapicontactspluginsymbian.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbian.qtplugin\"" \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbiansim/qmakepluginstubs/mobapicontactspluginsymbiansim.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbiansim.qtplugin\""

        symbiancntsim = \
            "\"$${EPOCROOT50}epoc32/release/armv5/urel/mobapicontactspluginsymbiansim.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbiansim.dll\"" \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbiansim/qmakepluginstubs/mobapicontactspluginsymbiansim.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbiansim.qtplugin\""

        contains(symbiancntsim_enabled, yes) {
            qtmobilitydeployment.pkg_postrules += symbiancntsim
        }

    }

    contains(mobility_modules, multimedia) {

        qtmobilitydeployment.sources += \
            $$(EPOCROOT50)epoc32/release/armv5/urel/QtMedia.dll \
            $$(EPOCROOT50)epoc32/release/armv5/urel/m3u.dll

        multimedia = \
            "IF package(0x1028315F)" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/QtMobilityMultimediaEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMultimediaEngine.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$$EPOCROOT32\epoc32/release/armv5/urel/QtMobilityMultimediaEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMultimediaEngine.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$$EPOCROOT31\epoc32/release/armv5/urel/QtMobilityMultimediaEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMultimediaEngine.dll\"" \
            "ELSE" \
            "   \"$$EPOCROOT50\epoc32/release/armv5/urel/QtMobilityMultimediaEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMultimediaEngine.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += multimedia

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/qmakepluginstubs/QtMobilityMultimediaEngine.qtplugin\" - \"!:\\resource\\qt\\plugins\\mediaservice\\QtMobilityMultimediaEngine.qtplugin\"" \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/m3u/qmakepluginstubs/m3u.qtplugin\"     - \"!:\\resource\\qt\\plugins\\playlistformats\\m3u.qtplugin\""
    }

    qtmobilitydeployment.pkg_postrules += pluginstubs

    qtmobilitydeployment.path = /sys/bin

    DEPLOYMENT += qtmobilitydeployment
}
