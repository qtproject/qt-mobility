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
                ../../src/location

include(../examples.pri)

CONFIG += mobility
MOBILITY = location

equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7){
    MOBILITY += bearer
    INCLUDEPATH += ../../src/bearer
} else {
    # use Bearer Management classes in QtNetwork module
    DEFINES += BEARER_IN_QTNETWORK
}

ICON = flickr_icon.svg


symbian|wince* {
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
    logfile.path = $$QT_MOBILITY_EXAMPLES
    logfile.files = nmealog.txt
    INSTALLS += logfile
}

wince*: {
    !contains(QT_CONFIG, no-jpeg): DEPLOYMENT_PLUGIN += qjpeg
}
