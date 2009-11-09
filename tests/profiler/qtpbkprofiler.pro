TEMPLATE = lib
TARGET = qtpbkprofiler

SOURCES += qtpbkprofiler.cpp
HEADERS += qtpbkprofiler.h

symbian {
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    deploy.path = $$EPOCROOT
    exportheaders.sources = $$HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders

    # This is for new exporting system coming in garden
    for(header, exportheaders.sources) {
        BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$exportheaders.path/$$basename(header)"
    }
}
