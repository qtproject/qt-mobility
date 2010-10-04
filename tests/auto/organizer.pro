include(../../staticconfig.pri)

requires(contains(mobility_modules,organizer))

TEMPLATE = subdirs
SUBDIRS += \
    qorganizercollection \
    qorganizeritem \
    qorganizeritemasync \
    qorganizeritemdetail \
    qorganizeritemdetaildefinition \
    qorganizeritemfilter \
    qorganizeritemmanager

