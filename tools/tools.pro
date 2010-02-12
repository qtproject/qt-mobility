include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += servicefw servicexmlgen icheck  #SFW API
contains(mobility_modules,publishsubscribe): SUBDIRS += vsexplorer qcrmlgen      #P&S API
