TEMPLATE = app
TARGET = qmlvideo

SOURCES += main.cpp
HEADERS += trace.h
RESOURCES += qmlvideo.qrc

qml_folder.source = qml/qmlvideo
qml_folder.target = qml
DEPLOYMENTFOLDERS = qml_folder

images_folder.source = images
images_folder.target = .
DEPLOYMENTFOLDERS += images_folder

SNIPPETS_PATH = ../snippets
include($$SNIPPETS_PATH/performancemonitor/performancemonitordeclarative.pri)
performanceItemAddDeployment()

symbian {
    TARGET.CAPABILITY += NetworkServices UserEnvironment
}

include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

