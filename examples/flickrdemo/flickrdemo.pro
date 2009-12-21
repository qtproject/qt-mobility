TEMPLATE = app
TARGET = FlickrDemo

FORMS += flickrdemo.ui \
         previewdialog.ui

QT      += network \
           xml

HEADERS += flickrdemo.h \
           xqlistwidget.h

SOURCES += flickrdemo.cpp \
           main.cpp \
           xqlistwidget.cpp

INCLUDEPATH += . ../../src/global \
                ../../src/bearer \
                ../../src/location

include(../examples.pri)

qtAddLibrary(QtLocation)
qtAddLibrary(QtBearer)

symbian:TARGET.CAPABILITY += Location \
    NetworkServices \
    ReadUserData

symbian: {
    license.depends = "\"install.txt\" - \"\", FILETEXT, TEXTEXIT"
    DEPLOYMENT += license
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

wince*: {
    !contains(QT_CONFIG, no-jpeg): DEPLOYMENT_PLUGIN += qjpeg
}
