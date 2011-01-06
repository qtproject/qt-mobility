TEMPLATE = subdirs

include(../../staticconfig.pri)

contains(mobility_modules,versit):contains(mobility_modules,organizer) {
   include(./contacts/organizer.pro)
}
