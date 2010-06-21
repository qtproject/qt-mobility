TEMPLATE = app
TARGET = weatherinfo_with_location

HEADERS = ../../examples/satellitedialog/satellitedialog.h \
            ../../examples/flickrdemo/connectivityhelper.h
SOURCES = weatherinfo.cpp \
            ../../examples/satellitedialog/satellitedialog.cpp \
            ../../examples/flickrdemo/connectivityhelper.cpp

RESOURCES = weatherinfo.qrc
QT += network svg

include(../../examples/examples.pri)

CONFIG += mobility
MOBILITY = location bearer

INCLUDEPATH += ../../src/global \
                ../../src/bearer \
                ../../src/location \
                ../../examples/satellitedialog \
                ../../examples/flickrdemo

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
