TEMPLATE=app
INCLUDEPATH += ../../src/location \
                ../../src/bearer \
                ../../src/global \
                ../satellitedialog


QT += webkit network

HEADERS = mapwindow.h \
            ../satellitedialog/satellitedialog.h
SOURCES = mapwindow.cpp \
            ../satellitedialog/satellitedialog.cpp \
            main.cpp

include(../examples.pri)

CONFIG += mobility
MOBILITY = location bearer

symbian|wince* {
    symbian {
        addFiles.sources = nmealog.txt
        DEPLOYMENT += addFiles
        TARGET.CAPABILITY += Location \
                NetworkServices
    }
    wince* {
        addFiles.sources = ./nmealog.txt
        addFiles.path = .
        DEPLOYMENT += addFiles
    }
} else {
    logfile.path = $$QT_MOBILITY_EXAMPLES
    logfile.files = nmealog.txt
    INSTALLS += logfile
}
