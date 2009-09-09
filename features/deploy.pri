
headers.files = $$PUBLIC_HEADERS
headers.path = $$QT_MOBILITY_INCLUDE

contains(TEMPLATE,lib) {
    target.path=$$QT_MOBILITY_LIB
    TARGET = $$qtLibraryTarget($$TARGET)
}
else:contains(TEMPLATE,app):target.path=$$QT_MOBILITY_BIN

INSTALLS+=headers target
