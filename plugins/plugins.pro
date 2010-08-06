include(../staticconfig.pri)

TEMPLATE = subdirs

contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,sensors): SUBDIRS += sensors
contains(mobility_modules,feedback): SUBDIRS += feedback
contains(mobility_modules,organizer): SUBDIRS += organizer
contains(mobility_modules,location): SUBDIRS += landmarks
contains(mobility_modules,location): SUBDIRS += geoservices
contains(mobility_modules,versit): SUBDIRS += versit

#disable declarative plugins on Symbian, do not compile yet                                    
!symbian:contains(QT_CONFIG,declarative): SUBDIRS += declarative
