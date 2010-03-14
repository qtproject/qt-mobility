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
MOBILITY = location
!maemo5:MOBILITY += bearer

symbian: {
    addFiles.sources = nmealog.txt
    DEPLOYMENT += addFiles
    
    TARGET.CAPABILITY = Location NetworkServices
} else {
    logfile.path = $$DESTDIR
    logfile.files = nmealog.txt
    logfile.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
    INSTALLS += logfile
    build_pass:ALL_DEPS+=install_logfile
}

