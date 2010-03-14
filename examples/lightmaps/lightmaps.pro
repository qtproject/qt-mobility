TEMPLATE = app
TARGET = lightmaps_with_location

HEADERS = ../satellitedialog/satellitedialog.h \
            ../flickrdemo/connectivityhelper.h
SOURCES = lightmaps.cpp \
            ../satellitedialog/satellitedialog.cpp \
            ../flickrdemo/connectivityhelper.cpp

QT += network

INCLUDEPATH += ../../src/global \
                ../../src/bearer \
                ../../src/location \
                ../satellitedialog \
                ../flickrdemo

include(../examples.pri)

symbian {
    TARGET.CAPABILITY = NetworkServices Location ReadUserData
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
}

CONFIG += mobility
MOBILITY = location bearer

symbian: {
    addFiles.sources = nmealog.txt
    DEPLOYMENT += addFiles
} else {
    logfile.path = $$DESTDIR
    logfile.files = nmealog.txt
    logfile.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
    INSTALLS += logfile
    build_pass:ALL_DEPS+=install_logfile
}
