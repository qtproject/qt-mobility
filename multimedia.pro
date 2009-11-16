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
    
    QtMobilityMultimediaEnginePlugin.sources = plugins/symbian/qmakepluginstubs/QtMobilityMultimediaEngine.qtplugin
    QtMobilityMultimediaEnginePlugin.path = /resource/qt/plugins/mediaservice
    
    DEPLOYMENT += QtMediaDeployment \
        QtMobilityMultimediaEnginePlugin
}