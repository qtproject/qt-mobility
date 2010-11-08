include(../../staticconfig.pri)

requires(contains(mobility_modules,organizer))

TEMPLATE = subdirs
SUBDIRS += \
    qorganizercollection \
    qorganizeritem \
    qorganizeritemasync \
    qorganizeritemdetail \
    qorganizeritemdetails \
    qorganizeritemdetaildefinition \
    qorganizeritemfilter \
    qorganizermanager

#contains(QT_CONFIG, declarative) {
#    SUBDIRS += qdeclarativeorganizer
#}
