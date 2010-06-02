TEMPLATE = subdirs

isEmpty(QT_LIBINFIX):symbian {
    include(../../staticconfig.pri)
    load(data_caging_paths)
    include($$QT_MOBILITY_BUILD_TREE/config.pri)

    SUBDIRS =
    TARGET = "QtMobility"
    TARGET.UID3 = 0x2002AC89

    VERSION = 1.0.1

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
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qsfwdatabasemanagerserver.exe

    contains(mobility_modules, location): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtLocation.dll

    contains(mobility_modules, systeminfo): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSystemInfo.dll

    contains(mobility_modules, publishsubscribe): qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtPublishSubscribe.dll \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qpspathmapperserver.exe

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
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += contacts

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbian/qmakepluginstubs/qtcontacts_symbian.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\qtcontacts_symbian.qtplugin\""

        contains(symbiancntsim_enabled, yes) {
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbiansim/qmakepluginstubs/qtcontacts_symbiansim.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\qtcontacts_symbiansim.qtplugin\""

            symbiancntsim = \
                "IF package(0x1028315F)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim.dll\"" \
                "ELSEIF package(0x102752AE)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim.dll\"" \
                "ELSEIF package(0x102032BE)" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim.dll\"" \
                "ELSE" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim.dll\"" \
                "ENDIF"

            qtmobilitydeployment.pkg_postrules += symbiancntsim
        }

    }

    contains(mobility_modules, multimedia) {

        qtmobilitydeployment.sources += \
            $$(EPOCROOT50)epoc32/release/$(PLATFORM)/$(TARGET)/QtMultimedia.dll \
            $$(EPOCROOT50)epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimedia_m3u.dll

        multimedia = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimedia_mmfengine.dll\" - \"!:\\sys\\bin\\qtmultimedia_mmfengine.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimedia_mmfengine.dll\" - \"!:\\sys\\bin\\qtmultimedia_mmfengine.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimedia_mmfengine.dll\" - \"!:\\sys\\bin\\qtmultimedia_mmfengine.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimedia_mmfengine.dll\" - \"!:\\sys\\bin\\qtmultimedia_mmfengine.dll\"" \
            "ENDIF"


        qtmobilitydeployment.pkg_postrules += multimedia

        contains(openmaxal_symbian_enabled, yes) {
            openmax = \
                "IF package(0x20022E6D)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimedia_xarecordservice.dll\" - \"!:\\sys\\bin\\qtmultimedia_xarecordservice.dll\"" \
                "ENDIF"

            qtmobilitydeployment.pkg_postrules += openmax

            pluginstubs += \
                "IF package(0x20022E6D)" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/openmaxal/mediarecorder/qmakepluginstubs/qtmultimedia_xarecordservice.qtplugin\" - \"!:\\resource\\qt\\plugins\\mediaservice\\qtmultimedia_xarecordservice.qtplugin\"" \
                "ENDIF"
        }

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/mmf/qmakepluginstubs/qtmultimedia_mmfengine.qtplugin\" - \"!:\\resource\\qt\\plugins\\mediaservice\\qtmultimedia_mmfengine.qtplugin\"" \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/m3u/qmakepluginstubs/qtmultimedia_m3u.qtplugin\"     - \"!:\\resource\\qt\\plugins\\playlistformats\\qtmultimedia_m3u.qtplugin\"" \
    }

    contains(mobility_modules, sensors) {

        qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSensors.dll

        sensors = ""

        equals(sensors_s60_31_enabled,yes) {
            sensors += \
                "IF package(0x102032BE)" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_s60sensorapi.dll\" - \"!:\\sys\\bin\\qtsensor_s60sensorapi.dll\"" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic.dll\" - \"!:\\sys\\bin\\qtsensors_generic.dll\"" \
                "ENDIF"
        } else:equals(sensors_symbian_enabled,yes) {
            sensors += \
                "IF package(0x102752AE)" \
                "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_sym.dll\" - \"!:\\sys\\bin\\qtsensors_sym.dll\"" \
                "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic.dll\" - \"!:\\sys\\bin\\qtsensors_generic.dll\"" \
                "ELSE" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_sym.dll\" - \"!:\\sys\\bin\\qtsensors_sym.dll\"" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic.dll\" - \"!:\\sys\\bin\\qtsensors_generic.dll\"" \
                "ENDIF"
        }

        !isEmpty(sensors):qtmobilitydeployment.pkg_postrules += sensors

        equals(sensors_s60_31_enabled,yes) {
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/s60_sensor_api/qmakepluginstubs/qtsensors_s60sensorapi.qtplugin\"  - \"!:\\resource\\qt\\plugins\\sensors\\qtsensors_s60sensorapi.qtplugin\""\
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/generic/qmakepluginstubs/qtsensors_generic.qtplugin\"  - \"!:\\resource\\qt\\plugins\\sensors\\qtsensors_generic.qtplugin\""
        } else:equals(sensors_symbian_enabled,yes) {
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/symbian/qmakepluginstubs/qtsensors_sym.qtplugin\"  - \"!:\\resource\\qt\\plugins\\sensors\\qtsensors_sym.qtplugin\""\
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/generic/qmakepluginstubs/qtsensors_generic.qtplugin\"  - \"!:\\resource\\qt\\plugins\\sensors\\qtsensors_generic.qtplugin\""
        }

    }

    !isEmpty(pluginstubs):qtmobilitydeployment.pkg_postrules += pluginstubs

    qtmobilitydeployment.path = /sys/bin

    DEPLOYMENT += qtmobilitydeployment
} else {
    message(Deployment of infixed library names not supported)
}
