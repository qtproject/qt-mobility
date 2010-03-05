include(../../../staticconfig.pri)

TEMPLATE = subdirs
contains(mobility_modules,contacts): SUBDIRS += qtcontactsdocsample
contains(mobility_modules,versit): SUBDIRS += qtversitdocsample
