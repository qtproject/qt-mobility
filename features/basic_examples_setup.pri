include(../staticconfig.pri)

win32:contains(CONFIG_WIN32,build_all):Win32DebugAndRelease=yes
mac | contains(Win32DebugAndRelease,yes) {
    #due to different debug/release library names we have to comply with 
    #whatever Qt does
    !contains(QT_CONFIG,debug)|!contains(QT_CONFIG,release) {
        CONFIG -= debug_and_release debug release
        contains(QT_CONFIG,debug): CONFIG+=debug
        contains(QT_CONFIG,release): CONFIG+=release
    }
}

CONFIG(debug, debug|release) {
    SUBDIRPART=Debug
} else {
    SUBDIRPART=Release
}

OUTPUT_DIR = $$QT_MOBILITY_BUILD_TREE
MOC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/moc
RCC_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/rcc
UI_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET/ui
OBJECTS_DIR = $$OUTPUT_DIR/build/$$SUBDIRPART/$$TARGET
mac:LIBS+= -F$$OUTPUT_DIR/lib
LIBS+= -L$$OUTPUT_DIR/lib
QMAKE_RPATHDIR+=$$QT_MOBILITY_LIB
INCLUDEPATH+= $$QT_MOBILITY_SOURCE_TREE/src/global

!plugin {
    target.path=$$QT_MOBILITY_EXAMPLES
} else {
    target.path = $${QT_MOBILITY_PLUGINS}/$${PLUGIN_TYPE}
}
INSTALLS += target

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
