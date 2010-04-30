include(../../../staticconfig.pri)

TEMPLATE = subdirs
contains(mobility_modules,contacts): SUBDIRS += qtcontactsdocsample
contains(mobility_modules,location): SUBDIRS += qtlandmarksdocsample
contains(mobility_modules,versit): SUBDIRS += qtversitdocsample
contains(mobility_modules,sensors): SUBDIRS += sensors
