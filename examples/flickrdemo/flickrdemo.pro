TEMPLATE = app
TARGET = flickrdemo

QT      += network \
           xml

HEADERS += flickrdemo.h \
           xqlistwidget.h \
           connectivityhelper.h

SOURCES += flickrdemo.cpp \
           main.cpp \
           xqlistwidget.cpp \ 
           connectivityhelper.cpp

INCLUDEPATH += . ../../src/global \
                ../../src/bearer \
                ../../src/location

include(../examples.pri)

CONFIG += mobility
MOBILITY = location bearer

ICON = flickr_icon.svg

symbian:TARGET.CAPABILITY += Location \
    NetworkServices \
    ReadUserData

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

wince*: {
    !contains(QT_CONFIG, no-jpeg): DEPLOYMENT_PLUGIN += qjpeg
}
