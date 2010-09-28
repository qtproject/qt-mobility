include(../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,sensors): SUBDIRS += sensors

# Disable declarative plug-ins. They are not supported
# until Mobility 1.1
#contains(QT_CONFIG,declarative): SUBDIRS += declarative
