include(../../staticconfig.pri)

requires(contains(mobility_modules,feedback))

TEMPLATE = subdirs
SUBDIRS += \
    qfeedbackactuator \
    qfeedbackhapticseffect \
    qfeedbackplugin

contains(QT_CONFIG, declarative) {
    SUBDIRS += qdeclarativefeedback
}
