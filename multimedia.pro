TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = \
    multimedia \
    plugins \
    examples \
    tests

plugins.depends = multimedia
examples.depends = multimedia
test.depends = multimedia

symbian {
    QtMediaDeployment.sources = QtMedia.dll \
        QtMobilityMultimediaEngine.dll
    QtMediaDeployment.path = /sys/bin
    DEPLOYMENT += QtMediaDeployment
}