load(basic_examples_setup)

!plugin {
    target.path=$$QT_MOBILITY_EXAMPLES
} else {
    target.path = $${QT_MOBILITY_PLUGINS}/$${PLUGIN_TYPE}
}
INSTALLS += target


