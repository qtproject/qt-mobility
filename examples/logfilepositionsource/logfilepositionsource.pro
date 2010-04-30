TEMPLATE=app
INCLUDEPATH += ../../src/location

HEADERS = logfilepositionsource.h \
          clientapplication.h
SOURCES = logfilepositionsource.cpp \
          clientapplication.cpp \
          main.cpp

CONFIG += console

include(../examples.pri)

CONFIG += mobility
MOBILITY = location

symbian|wince*: {
    symbian: {
        addFiles.sources = simplelog.txt
        DEPLOYMENT += addFiles
    
        TARGET.CAPABILITY = Location
    }
    wince*: {
        addFiles.sources = ./simplelog.txt
        addFiles.path = .
        DEPLOYMENT += addFiles
    }
} else {
    logfileexample.path = $$DESTDIR
    logfileexample.files = simplelog.txt
    logfileexample.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
    INSTALLS += logfileexample
    build_pass:ALL_DEPS+=install_logfileexample
}

