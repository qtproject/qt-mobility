TEMPLATE = app
TARGET = qmlvideofx
QT += opengl

SOURCES += main.cpp
HEADERS += trace.h
RESOURCES += qmlvideofx.qrc

qml_folder.source = qml/qmlvideofx
qml_folder.target = qml
DEPLOYMENTFOLDERS += qml_folder

images_folder.source = images
images_folder.target = .
DEPLOYMENTFOLDERS += images_folder

SNIPPETS_PATH = ../snippets
include($$SNIPPETS_PATH/performancemonitor/performancemonitordeclarative.pri)
performanceItemAddDeployment()

symbian|contains(MEEGO_EDITION,harmattan): {
    DEFINES += SMALL_SCREEN_LAYOUT
    DEFINES += SMALL_SCREEN_PHYSICAL
}

symbian {
    DEFINES += USE_OPENGL_GRAPHICSSYSTEM
    TARGET.CAPABILITY += UserEnvironment
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

