#for now we do not actually have anything to build
#just ensure installation of public headers
TEMPLATE = subdirs

include(../../common.pri)
PUBLIC_HEADERS += qmobilityglobal.h

headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE
INSTALLS+= headers

symbian {
    path=$$MW_LAYER_PUBLIC_EXPORT_PATH("")

    exportPath=$$EPOCROOT"."$$dirname(path)
    nativePath=$$replace(exportPath, /,\)
    exists($$nativePath) {
    } else {
        system($$QMAKE_MKDIR $$nativePath)
    }

    for(header, headers.files) {
        BLD_INF_RULES.prj_exports += "$$header $$MW_LAYER_PUBLIC_EXPORT_PATH($$basename(header))"
    }
}
