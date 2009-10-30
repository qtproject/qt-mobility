headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE

contains(TEMPLATE,.*lib) {
    target.path=$$QT_MOBILITY_LIB
    TARGET = $$qtLibraryTarget($$TARGET)
} else {
    contains(TEMPLATE,.*app):target.path=$$QT_MOBILITY_BIN
}

INSTALLS+=headers target

mac:contains(QT_CONFIG,qt_framework) {
#CONFIG += lib_bundle absolute_library_soname
    CONFIG += lib_bundle
    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $${PUBLIC_HEADERS}
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
}


