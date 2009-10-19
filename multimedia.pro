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

symbian: {
    load(data_caging_paths)
    cameratest.sources = QtMedia.dll QtMultimedia.dll
    qtmedia.path = "/sys/bin"

    DEPLOYMENT += qtmedia
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB -DRM

}