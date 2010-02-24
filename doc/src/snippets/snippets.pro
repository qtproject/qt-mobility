include(../../../staticconfig.pri)

TEMPLATE = subdirs
contains(mobility_modules,contacts): SUBDIRS += qtcontactsdocsample
