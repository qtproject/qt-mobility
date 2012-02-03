TEMPLATE = subdirs

symbian {
    include(../../staticconfig.pri)
    include(../../features/utils.pri)
    load(data_caging_paths)
    include($$QT_MOBILITY_BUILD_TREE/config.pri)

    SUBDIRS =
    TARGET = "QtMobility$${QT_LIBINFIX}"
    TARGET.UID3 = $$mobilityUID(0x2002AC89)

    VERSION = 1.2.2

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
    epoc51 = $$(EPOCROOT51)
    epocS3PS1 = $$(EPOCROOT_S3PS1)
    epocS3PS2 = $$(EPOCROOT_S3PS2)

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
    #Epocroot 51 is based on a N97 sdk
    isEmpty(epoc51) {
        EPOCROOT51 = $${EPOCROOT}
    } else {
        EPOCROOT51 = $$(EPOCROOT51)
    }
    isEmpty(epocS3PS1) {
        EPOCROOT_S3PS1 = $${EPOCROOT}
    } else {
        EPOCROOT_S3PS1 = $$(EPOCROOT_S3PS1)
    }
    isEmpty(epocS3PS2) {
        EPOCROOT_S3PS2 = $${EPOCROOT}
    } else {
        EPOCROOT_S3PS2 = $$(EPOCROOT_S3PS2)
    }

    #Symbian^3 and beyond requires special package flags
    #we cannot use S60_VERSION == 5.2 as Qt 4.6.x does not define it yet
    #see $QTDIR/mkspecs/common/symbian/symbian.conf for details
    exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.2.sis)|exists($${EPOCROOT}epoc32/data/z/system/install/series60v5.2.sis)|exists($${EPOCROOT}epoc32/release/armv5/lib/libstdcppv5.dso) {
        pkg_version = $$replace(VERSION,"\\.",",")
        qtmobilitydeployment.pkg_prerules += "$${LITERAL_HASH}{\"QtMobility$${QT_LIBINFIX}\"},($${TARGET.UID3}),$${pkg_version},TYPE=SA,RU,NR"
    }

    contains(mobility_modules, messaging) {
        qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtMessaging$${QT_LIBINFIX}.dll

        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_messaging$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\messaging\\qmakepluginstubs\\declarative_messaging$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\messaging\\declarative_messaging$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\messaging\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\messaging\\qmldir\""
        }
    }
    
    contains(mobility_modules, connectivity) {
        # QtConnectivity needs to be built and deployed to each Symbian version differently. This
        # is because NFC support is only available from Symbian^3 PS2 onwards. There are also some
        # differences with Bluetooth, hence the Symbian^1 variants.
        #
        # - PS2 build has NFC and Bluetooth support, requires Symbian^3 device with PS2 firmware
        # - PS1 build has Bluetooth support only (NFC is a stub), requires Symbian^3 device with
        #   PS1 firmware
        #   Installing Qt Mobility on a Symbian^3 PS1 device then upgrading device firmware to PS2
        #   will leave Qt Mobility NFC support disabled until Qt Mobility is reinstalled/upgraded.
        # - S60 5.0 build has Bluetooth support only (NFC is a stub)
        # - S60 3.1 and 3.2 build only produces a stub library.
        connectivity = \
            "IF package(0x20022E6D) AND exists(\"z:\\sys\\bin\\nfc.dll\")" \
            "    \"$${EPOCROOT_S3PS2}epoc32/release/$(PLATFORM)/$(TARGET)/QtConnectivity$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtConnectivity$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x20022E6D)" \
            "    \"$${EPOCROOT_S3PS1}epoc32/release/$(PLATFORM)/$(TARGET)/QtConnectivity$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtConnectivity$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x1028315F)" \
            "    \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtConnectivity$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtConnectivity$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE) OR package(0x102032BE)" \
            "    \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/QtConnectivity$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtConnectivity$${QT_LIBINFIX}.dll\"" \
            "ELSE" \
            "    \"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/QtConnectivity$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtConnectivity$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_connectivity$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\connectivity\\qmakepluginstubs\\declarative_connectivity$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\connectivity\\declarative_connectivity$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\connectivity\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\connectivity\\qmldir\""
        }


        qtmobilitydeployment.pkg_postrules += connectivity
    }

    contains(mobility_modules, serviceframework) { 
        qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtServiceFramework$${QT_LIBINFIX}.dll \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qsfwdatabasemanagerserver$${QT_LIBINFIX}.exe
        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_serviceframework$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\serviceframework\\qmakepluginstubs\\declarative_serviceframework$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\serviceframework\\declarative_serviceframework$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\serviceframework\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\serviceframework\\qmldir\""
        }
    }

    contains(mobility_modules, location) {
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtLocation$${QT_LIBINFIX}.dll
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtgeoservices_nokia$${QT_LIBINFIX}.dll
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtlandmarks_symbian$${QT_LIBINFIX}.dll
        pluginstubs += "\"$$QT_MOBILITY_BUILD_TREE/plugins/geoservices/nokia/qmakepluginstubs/qtgeoservices_nokia$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\geoservices\\qtgeoservices_nokia$${QT_LIBINFIX}.qtplugin\""
        pluginstubs += "\"$$QT_MOBILITY_BUILD_TREE/plugins/landmarks/symbian_landmarks/qmakepluginstubs/qtlandmarks_symbian$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\landmarks\\qtlandmarks_symbian$${QT_LIBINFIX}.qtplugin\""
        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_location$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\location\\qmakepluginstubs\\declarative_location$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\location\\declarative_location$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\location\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\location\\qmldir\""
        }
    }

    contains(mobility_modules, systeminfo) { 
        sysinfo = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSystemInfo$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtSystemInfo$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/QtSystemInfo$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtSystemInfo$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/QtSystemInfo$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtSystemInfo$${QT_LIBINFIX}.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSystemInfo$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtSystemInfo$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += sysinfo
        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_systeminfo$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\systeminfo\\qmakepluginstubs\\declarative_systeminfo$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\systeminfo\\declarative_systeminfo$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\systeminfo\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\systeminfo\\qmldir\""
        }
    }

    contains(mobility_modules, publishsubscribe) {
        qtmobilitydeployment.sources += \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtPublishSubscribe$${QT_LIBINFIX}.dll \
        $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qpspathmapperserver$${QT_LIBINFIX}.exe
        contains(QT_CONFIG, declarative) {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_publishsubscribe$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\publishsubscribe\\qmakepluginstubs\\declarative_publishsubscribe$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\publishsubscribe\\declarative_publishsubscribe$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\publishsubscribe\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\publishsubscribe\\qmldir\""
        }
    }

    contains(mobility_modules, versit) {
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtVersit$${QT_LIBINFIX}.dll
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtversit_backuphandler$${QT_LIBINFIX}.dll
        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\versit\\backuphandler\\qmakepluginstubs\\qtversit_backuphandler$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\versit\\qtversit_backuphandler$${QT_LIBINFIX}.qtplugin\""

        ## now the versit organizer module - depends on versit and organizer.
        contains(mobility_modules, organizer) {
            qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtVersitOrganizer$${QT_LIBINFIX}.dll
        }
    }

    contains(mobility_modules, feedback) {
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtFeedback$${QT_LIBINFIX}.dll
        contains(immersion_enabled, yes) {
            qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtfeedback_immersion$${QT_LIBINFIX}.dll
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\feedback\\immersion\\qmakepluginstubs\\qtfeedback_immersion$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\feedback\\qtfeedback_immersion$${QT_LIBINFIX}.qtplugin\""
        }

        equals(QT_MAJOR_VERSION, 4) : greaterThan(QT_MINOR_VERSION, 6):contains(mobility_modules,multimedia) {
            qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtfeedback_mmk$${QT_LIBINFIX}.dll
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\feedback\\mmk\\qmakepluginstubs\\qtfeedback_mmk$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\feedback\\qtfeedback_mmk$${QT_LIBINFIX}.qtplugin\""
        }

        feedback = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtfeedback_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtfeedback_symbian$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtfeedback_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtfeedback_symbian$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtfeedback_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtfeedback_symbian$${QT_LIBINFIX}.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtfeedback_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtfeedback_symbian$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += feedback

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\feedback\\symbian\\qmakepluginstubs\\qtfeedback_symbian$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\feedback\\qtfeedback_symbian$${QT_LIBINFIX}.qtplugin\""

        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
                $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_feedback$${QT_LIBINFIX}.dll
            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\feedback\\qmakepluginstubs\\declarative_feedback$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\feedback\\declarative_feedback$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
                "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\feedback\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\feedback\\qmldir\""
        }
    }

    contains(mobility_modules, organizer) { 
        qtmobilitydeployment.sources += $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtOrganizer$${QT_LIBINFIX}.dll   
        organizer = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtorganizer_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtorganizer_symbian$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtorganizer_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtorganizer_symbian$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtorganizer_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtorganizer_symbian$${QT_LIBINFIX}.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtorganizer_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtorganizer_symbian$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += organizer

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\organizer\\symbian\\qmakepluginstubs\\qtorganizer_symbian$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\organizer\\qtorganizer_symbian$${QT_LIBINFIX}.qtplugin\""
     contains(QT_CONFIG, declarative):contains(mobility_modules,versit)  {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_organizer$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\organizer\\qmakepluginstubs\\declarative_organizer$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\organizer\\declarative_organizer$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\organizer\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\organizer\\qmldir\""
        }
    }

    contains(mobility_modules, gallery) {
        #QDocumentGallery on S60/Symbian relies on MetaDataSystem. There exists few different versions of it and we must
        #check what version is currently installed on devices. Check is made a with known dlls.
        #Installation has these preconditions:
        # QDocumentGallery built against EPOCROOT50 has mds 2.0 libs in place (3.1/3.2/5.0)
        # QDocumentGallery built against EPOCROOT51 has mds 2.5 libs in place (Symbian^3 and N97)
        # QDocumentGallery built against EPOCROOT32 has no mds libs at all (stub implementation, api only)
        # Also if checked mds library is present on c-drive then also check whether mds is installed
        gallerymdscheck = \
            "if exists(\"z:\sys\bin\locationutility.dll\")" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtGallery$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtGallery$${QT_LIBINFIX}.dll\"" \
            "elseif exists(\"c:\sys\bin\locationutility.dll\") AND package(0x200009F5)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtGallery$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtGallery$${QT_LIBINFIX}.dll\"" \
            "elseif exists(\"z:\sys\bin\locationmanagerserver.exe\")" \
            "   \"$${EPOCROOT51}epoc32/release/$(PLATFORM)/$(TARGET)/QtGallery$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtGallery$${QT_LIBINFIX}.dll\"" \
            "elseif exists(\"c:\sys\bin\locationmanagerserver.exe\") AND package(0x200009F5)" \
            "   \"$${EPOCROOT51}epoc32/release/$(PLATFORM)/$(TARGET)/QtGallery$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtGallery$${QT_LIBINFIX}.dll\"" \
            "else" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/QtGallery$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtGallery$${QT_LIBINFIX}.dll\"" \
            "endif"

        qtmobilitydeployment.pkg_postrules += gallerymdscheck

        # QDocumentGallery QML plugin
        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_gallery$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\gallery\\qmakepluginstubs\\declarative_gallery$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\gallery\\declarative_gallery$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\gallery\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\gallery\\qmldir\""
        }
    }

    contains(mobility_modules, bearer) {
        bearer = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer$${QT_LIBINFIX}.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += bearer
    }
    
    contains(mobility_modules, bearer) {
            !contains(MOBILITY_SD_MCL_BUILD, yes):exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.2.sis)|exists($${EPOCROOT}epoc32/data/z/system/install/series60v5.2.sis)|exists($${EPOCROOT}epoc32/release/armv5/lib/libstdcppv5.dso) {
            bearer10_0 = \ 
        "IF package(0x1028315F)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer{000a0000}$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer{000a0000}$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x102752AE)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer{000a0000}$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer{000a0000}$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x102032BE)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer{000a0000}$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer{000a0000}$${QT_LIBINFIX}.dll\"" \
                "ELSE" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtBearer{000a0000}$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\QtBearer{000a0000}$${QT_LIBINFIX}.dll\"" \
                "ENDIF"
            qtmobilitydeployment.pkg_postrules += bearer10_0
        }
    }

    contains(mobility_modules, contacts) {

        qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtContacts$${QT_LIBINFIX}.dll
        contains(mobility_modules,serviceframework):qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_serviceactionmanager$${QT_LIBINFIX}.dll

        !exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.4.sis){
            contacts = \
                "IF package(0x1028315F)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x102752AE)" \
                "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x102032BE)" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian$${QT_LIBINFIX}.dll\"" \
                "ELSE" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbian$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbian$${QT_LIBINFIX}.dll\"" \
                "ENDIF"

            qtmobilitydeployment.pkg_postrules += contacts

            pluginstubs += \
                "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbian/plugin/qmakepluginstubs/qtcontacts_symbian$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\contacts\\qtcontacts_symbian$${QT_LIBINFIX}.qtplugin\""
        }

        contains(mobility_modules,serviceframework):pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/serviceactionmanager/qmakepluginstubs/qtcontacts_serviceactionmanager$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\contacts\\qtcontacts_serviceactionmanager$${QT_LIBINFIX}.qtplugin\""

        !exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.4.sis){
            contains(symbiancntsim_enabled, yes) {
                pluginstubs += \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/contacts/symbiansim/qmakepluginstubs/qtcontacts_symbiansim$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt\\plugins\\contacts\\qtcontacts_symbiansim$${QT_LIBINFIX}.qtplugin\""

                symbiancntsim = \
                    "IF package(0x1028315F)" \
                    "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim$${QT_LIBINFIX}.dll\"" \
                    "ELSEIF package(0x102752AE)" \
                    "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim$${QT_LIBINFIX}.dll\"" \
                    "ELSEIF package(0x102032BE)" \
                    "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim$${QT_LIBINFIX}.dll\"" \
                    "ELSE" \
                    "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtcontacts_symbiansim$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtcontacts_symbiansim$${QT_LIBINFIX}.dll\"" \
                    "ENDIF"

                qtmobilitydeployment.pkg_postrules += symbiancntsim
            }
        }
     contains(QT_CONFIG, declarative):contains(mobility_modules,versit)  {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_contacts$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\contacts\\qmakepluginstubs\\declarative_contacts$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\contacts\\declarative_contacts$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\contacts\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\contacts\\qmldir\""
        }
    }

    contains(mobility_modules, multimedia) {

        qtmobilitydeployment.sources += \
            $$(EPOCROOT50)epoc32/release/$(PLATFORM)/$(TARGET)/QtMultimediaKit$${QT_LIBINFIX}.dll \
            $$(EPOCROOT50)epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_m3u$${QT_LIBINFIX}.dll

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/m3u/qmakepluginstubs/qtmultimediakit_m3u$${QT_LIBINFIX}.qtplugin\"     - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\playlistformats\\qtmultimediakit_m3u$${QT_LIBINFIX}.qtplugin\""

        contains(openmaxal_symbian_enabled, yes) {
            openmax = \
                "\"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_openmaxalengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_openmaxalengine$${QT_LIBINFIX}.dll\""

            qtmobilitydeployment.pkg_postrules += openmax

            pluginstubs += \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/openmaxal/qmakepluginstubs/qtmultimediakit_openmaxalengine$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\mediaservice\\qtmultimediakit_openmaxalengine$${QT_LIBINFIX}.qtplugin\""
        } else {

            multimedia = \
                "IF package(0x1028315F)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x102752AE)" \
                "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x102032BE)" \
                "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\"" \
                "ELSEIF package(0x20022E6D)" \
                "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.dll\"" \
                "ENDIF"

            qtmobilitydeployment.pkg_postrules += multimedia

            pluginstubs += \
                "IF package(0x1028315F)" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/mmf/qmakepluginstubs/qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\mediaservice\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\"" \
                "ELSEIF package(0x102752AE)" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/mmf/qmakepluginstubs/qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\mediaservice\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\"" \
                "ELSEIF package(0x102032BE)" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/mmf/qmakepluginstubs/qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\mediaservice\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\"" \
                "ELSEIF package(0x20022E6D)" \
                    "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/mmf/qmakepluginstubs/qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\mediaservice\\qtmultimediakit_mmfengine$${QT_LIBINFIX}.qtplugin\"" \
                "ENDIF"
        }

        camera = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x20022E6D)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtmultimediakit_ecamengine$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        qtmobilitydeployment.pkg_postrules += camera

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/multimedia/symbian/ecam/qmakepluginstubs/qtmultimediakit_ecamengine$${QT_LIBINFIX}.qtplugin\" - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\mediaservice\\qtmultimediakit_ecamengine$${QT_LIBINFIX}.qtplugin\""


    contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_multimedia$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\multimedia\\qmakepluginstubs\\declarative_multimedia$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMultimediaKit\\declarative_multimedia$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\multimedia\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMultimediaKit\\qmldir\""
        }
    }

    contains(mobility_modules, sensors) {

        equals(sensors_symbian_enabled,yes) {
            sensorplugin=symbian
        } else:equals(sensors_s60_31_enabled,yes) {
            sensorplugin=s60_sensor_api
        } else {
            error("Must have a Symbian sensor backend available")
        }

        qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/QtSensors$${QT_LIBINFIX}.dll

        sensors = \
            "IF package(0x1028315F)" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_sym$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_sym$${QT_LIBINFIX}.dll\"" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_generic$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102752AE)" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_sym$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_sym$${QT_LIBINFIX}.dll\"" \
            "   \"$${EPOCROOT32}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_generic$${QT_LIBINFIX}.dll\"" \
            "ELSEIF package(0x102032BE)" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_sym$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_sym$${QT_LIBINFIX}.dll\"" \
            "   \"$${EPOCROOT31}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_generic$${QT_LIBINFIX}.dll\"" \
            "ELSE" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_sym$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_sym$${QT_LIBINFIX}.dll\"" \
            "   \"$${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/qtsensors_generic$${QT_LIBINFIX}.dll\" - \"!:\\sys\\bin\\qtsensors_generic$${QT_LIBINFIX}.dll\"" \
            "ENDIF"

        pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/$$sensorplugin/qmakepluginstubs/qtsensors_sym$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\sensors\\qtsensors_sym$${QT_LIBINFIX}.qtplugin\"" \
            "\"$$QT_MOBILITY_BUILD_TREE/plugins/sensors/generic/qmakepluginstubs/qtsensors_generic$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\plugins\\sensors\\qtsensors_generic$${QT_LIBINFIX}.qtplugin\""

        !isEmpty(sensors):qtmobilitydeployment.pkg_postrules += sensors
        contains(QT_CONFIG, declarative): {
            qtmobilitydeployment.sources += \
            $${EPOCROOT50}epoc32/release/$(PLATFORM)/$(TARGET)/declarative_sensors$${QT_LIBINFIX}.dll
            pluginstubs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\sensors\\qmakepluginstubs\\declarative_sensors$${QT_LIBINFIX}.qtplugin\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\sensors\\declarative_sensors$${QT_LIBINFIX}.qtplugin\""
            qmldirs += \
            "\"$$QT_MOBILITY_BUILD_TREE\\plugins\\declarative\\sensors\\qmldir\"  - \"!:\\resource\\qt$${QT_LIBINFIX}\\imports\\QtMobility\\sensors\\qmldir\""
        }
    }

    !isEmpty(pluginstubs):qtmobilitydeployment.pkg_postrules += pluginstubs
    !isEmpty(qmldirs):qtmobilitydeployment.pkg_postrules += qmldirs
 
    qtmobilitydeployment.path = /sys/bin

    # Support backup and restore for QtMobility libraries and applications
    mobilitybackup.sources = backup_registration.xml
    mobilitybackup.path = c:/private/10202d56/import/packages/$$replace(TARGET.UID3, 0x,)

    DEPLOYMENT += qtmobilitydeployment\
                mobilitybackup

    exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.3.sis) {
        !exists($${EPOCROOT}epoc32/release/winscw/udeb/z/system/install/series60v5.4.sis) {
            # Export qtmobilitycontactssrv.iby only in Belle not in future releases
            # Iby file will be part of new contactsrv pkg after Belle
            BLD_INF_RULES.prj_exports += \
                "$${LITERAL_HASH}include <platform_paths.hrh>" \
                "deviceconfiguration/qtmobilitycontactssrv.iby  CORE_MW_LAYER_IBY_EXPORT_PATH(qtmobilitycontactssrv.iby)"
        }
    }
}

