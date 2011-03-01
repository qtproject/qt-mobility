exists($$PWD/maemo5pkgrules.pri): exists($$PWD/symbianpkgrules.pri) {
    symbian: include($$PWD/symbianpkgrules.pri)
    maemo5: include($$PWD/maemo5pkgrules.pri)

    maemo6 {
        DEFINES+= Q_WS_MAEMO_6
        DEFINES+= QTM_EXAMPLES_SMALL_SCREEN
        DEFINES+= QTM_EXAMPLES_PREFER_LANDSCAPE
    }
    maemo5 {
        DEFINES+= Q_WS_MAEMO_5
        DEFINES+= QTM_EXAMPLES_SMALL_SCREEN
        DEFINES+= QTM_EXAMPLES_PREFER_LANDSCAPE
    }
    symbian {
        DEFINES+= QTM_EXAMPLES_SMALL_SCREEN
    }
} else {
    include(../features/basic_examples_setup.pri)
}

!plugin {
    target.path=$$QT_MOBILITY_EXAMPLES
} else {
    target.path = $${QT_MOBILITY_PLUGINS}/$${PLUGIN_TYPE}
}

INSTALLS += target


