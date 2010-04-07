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
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtMessaging.dll

    contains(mobility_modules, serviceframework): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtServiceFramework.dll \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/SFWDatabaseManagerServer.exe

    contains(mobility_modules, location): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtLocation.dll

    contains(mobility_modules, systeminfo): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSystemInfo.dll

    contains(mobility_modules, publishsubscribe): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtPublishSubscribe.dll \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/PSPathMapperServer.exe

    contains(mobility_modules, versit): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtVersit.dll


    contains(mobility_modules, bearer) {
        bearer = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer.dll\" - \"!:\\sys\\bin\\QtBearer.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += bearer
    }

    contains(mobility_modules, contacts) {

        qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtContacts.dll

        contacts = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbian.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbian.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += contacts

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbian/qmakepluginstubs/mobapicontactspluginsymbian.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbian.qtplugin\""

        contains(symbiancntsim_enabled, yes) {
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbiansim/qmakepluginstubs/mobapicontactspluginsymbiansim.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\mobapicontactspluginsymbiansim.qtplugin\""

            symbiancntsim = \
                "IF package(0x1028315F)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbiansim.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbiansim.dll\"" \
                "ELSEIF package(0x102752AE)" \
                "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbiansim.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbiansim.dll\"" \
                "ELSEIF package(0x102032BE)" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbiansim.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbiansim.dll\"" \
                "ELSE" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/mobapicontactspluginsymbiansim.dll\" - \"!:\\sys\\bin\\mobapicontactspluginsymbiansim.dll\"" \
                "ENDIF"

            qtmobilitydeployment.pkg_postrules += symbiancntsim
        }

    }

    contains(mobility_modules, multimedia) {

        qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtMedia.dll \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/m3u.dll

        multimedia = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtMobilityMmfEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMmfEngine.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/QtMobilityMmfEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMmfEngine.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/QtMobilityMmfEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMmfEngine.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtMobilityMmfEngine.dll\" - \"!:\\sys\\bin\\QtMobilityMmfEngine.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += multimedia

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/mmf/qmakepluginstubs/QtMobilityMmfEngine.qtplugin\" - \"!:\\resource\\qt\\plugins\\mediaservice\\QtMobilityMmfEngine.qtplugin\"" \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/m3u/qmakepluginstubs/m3u.qtplugin\"     - \"!:\\resource\\qt\\plugins\\playlistformats\\m3u.qtplugin\""
    }

    contains(mobility_modules, sensors) {

        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSensors.dll

        equals($${EPOCROOT50}, $${EPOCROOT32}):equals($${EPOCROOT32}, $${EPOCROOT31}) {
            contains(S60_VERSION, 3.1) {
                sensors = \
                    "IF package(0x102032BE)" \
                    "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/sensor_s60sensorapi.dll\" - \"!:\\sys\\bin\\sensor_s60sensorapi.dll\"" \
                    "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_generic.dll\" - \"!:\\sys\\bin\\sensors_generic.dll\"" \
                    "ENDIF"
           } else {
                sensors = \
                    "IF package(0x1028315F)" \
                    "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_sym.dll\" - \"!:\\sys\\bin\\sensors_sym.dll\"" \
                    "ELSEIF package(0x102752AE)" \
                    "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_sym.dll\" - \"!:\\sys\\bin\\sensors_sym.dll\"" \
                    "ELSE" \
                    "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_sym.dll\" - \"!:\\sys\\bin\\sensors_sym.dll\"" \
                    "ENDIF"
            }
        } else {
            sensors = \
                "IF package(0x1028315F)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_sym.dll\" - \"!:\\sys\\bin\\sensors_sym.dll\"" \
                "ELSEIF package(0x102752AE)" \
                "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_sym.dll\" - \"!:\\sys\\bin\\sensors_sym.dll\"" \
                "ELSEIF package(0x102032BE)" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/sensor_s60sensorapi.dll\" - \"!:\\sys\\bin\\sensor_s60sensorapi.dll\"" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_generic.dll\" - \"!:\\sys\\bin\\sensors_generic.dll\"" \
                "ELSE" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/sensors_sym.dll\" - \"!:\\sys\\bin\\sensors_sym.dll\"" \
                "ENDIF"
        }

        qtmobilitydeployment.pkg_postrules += sensors

        equals($${EPOCROOT50}, $${EPOCROOT32}):equals($${EPOCROOT32}, $${EPOCROOT31}) {
            contains(S60_VERSION, 3.1) {
                pluginstubs += \
                    "IF package(0x102032BE)" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/s60_sensor_api/qmakepluginstubs/sensor_s60sensorapi.qtplugin\" - \"!:\\resource\\qt\\plugins\\sensors\\sensor_s60sensorapi.qtplugin\"" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/generic/qmakepluginstubs/sensors_generic.qtplugin\" - \"!:\\resource\\qt\\plugins\\sensors\\sensors_generic.qtplugin\"" \
                    "ENDIF"
            }
        } else {
            pluginstubs += \
                "IF package(0x102032BE)" \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/s60_sensor_api/qmakepluginstubs/sensor_s60sensorapi.qtplugin\" - \"!:\\resource\\qt\\plugins\\sensors\\sensor_s60sensorapi.qtplugin\"" \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/generic/qmakepluginstubs/sensors_generic.qtplugin\" - \"!:\\resource\\qt\\plugins\\sensors\\sensors_generic.qtplugin\"" \
                "ENDIF"
        }
    }

    qtmobilitydeployment.pkg_postrules += pluginstubs

    qtmobilitydeployment.path = /sys/bin

    DEPLOYMENT += qtmobilitydeployment
}
