include(../../staticconfig.pri)

requires(contains(mobility_modules,versit))

TEMPLATE = subdirs
SUBDIRS += \
    qvcard21writer \
    qvcard30writer \
    qversitcontactexporter \
    qversitcontactimporter \
    qversitcontactplugins \
    qversitdocument \
    qversitproperty \
    qversitreader \
    qversitwriter

contains(mobility_modules,organizer) {
    SUBDIRS += \
        qversitorganizerexporter \
        qversitorganizerimporter \
        qversit
}

