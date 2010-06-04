headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE

contains(TEMPLATE,.*lib) {
    target.path=$$QT_MOBILITY_LIB
    TARGET = $$qtLibraryTarget($${TARGET}$${QT_LIBINFIX})
 
    symbian {
        middleware {  path=$$MW_LAYER_PUBLIC_EXPORT_PATH("") }
        app {  path=$$APP_LAYER_PUBLIC_EXPORT_PATH("") }

        exportPath=$$EPOCROOT"."$$dirname(path)
        nativePath=$$replace(exportPath, /,\)
        exists($$nativePath) {
        } else {
            system($$QMAKE_MKDIR $$nativePath)
        }
 
        for(header, headers.files) {
            middleware {  BLD_INF_RULES.prj_exports += "$$header $$MW_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"}
            app {  BLD_INF_RULES.prj_exports += "$$header $$APP_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"}
        }

    }
 
} else {
    contains(TEMPLATE,.*app):target.path=$$QT_MOBILITY_BIN
}

INSTALLS+=target headers

mac:contains(QT_CONFIG,qt_framework) {
    CONFIG += lib_bundle absolute_library_soname
    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $${PUBLIC_HEADERS}
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
}

CONFIG+= create_prl
