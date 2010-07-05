TEMPLATE = app
TARGET = weatherinfo_with_location

HEADERS = ../satellitedialog/satellitedialog.h \
            ../flickrdemo/connectivityhelper.h
SOURCES = weatherinfo.cpp \
            ../satellitedialog/satellitedialog.cpp \
            ../flickrdemo/connectivityhelper.cpp

RESOURCES = weatherinfo.qrc
QT += network svg

include(../examples.pri)

CONFIG += mobility
MOBILITY = location bearer

INCLUDEPATH += ../../src/global \
                ../../src/bearer \
                ../../src/location \
                ../satellitedialog \
                ../flickrdemo

symbian {
    symbian {
        addFiles.sources = nmealog.txt
        DEPLOYMENT += addFiles
        TARGET.CAPABILITY += Location \
                NetworkServices \
                ReadUserData
    }
    wince* {
        addFiles.sources = ./nmealog.txt
        addFiles.path = .
        DEPLOYMENT += addFiles
    }
} else {
    logfile.path = $$QT_MOBILITY_PREFIX/bin
    logfile.files = nmealog.txt
    INSTALLS += logfile
}
