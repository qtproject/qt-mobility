include(../features/basic_examples_setup.pri)

!plugin {
    target.path=$$QT_MOBILITY_EXAMPLES
} else {
    target.path = $${QT_MOBILITY_PLUGINS}/$${PLUGIN_TYPE}
}

symbian: {
    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "
    examples_deployment.pkg_prerules += vendorinfo
    DEPLOYMENT += examples_deployment
}

INSTALLS += target


