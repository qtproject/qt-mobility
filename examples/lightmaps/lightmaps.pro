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

CONFIG += mobility
MOBILITY = location bearer

symbian|wince* {
    symbian {
        addFiles.sources = nmealog.txt
        DEPLOYMENT += addFiles
        TARGET.CAPABILITY = NetworkServices Location ReadUserData
        TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    }
    wince*: {
        addFiles.sources = ./nmealog.txt
        addFiles.path = .
        DEPLOYMENT += addFiles
    }
} else {
    logfile.path = $$QT_MOBILITY_PREFIX/bin
    logfile.files = nmealog.txt
    INSTALLS += logfile
}
