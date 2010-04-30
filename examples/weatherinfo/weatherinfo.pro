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
    TARGET.CAPABILITY = NetworkServices Location ReadUserData
}

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
