#for now we don't actually have anything to build
#just ensure installation of public headers
TEMPLATE = subdirs

include(../../common.pri)
PUBLIC_HEADERS += qmobilityglobal.h

headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE
INSTALLS+= headers

symbian {
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include/mw

    #export headers into EPOCROOT
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }
}


