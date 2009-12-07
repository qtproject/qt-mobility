TEMPLATE = subdirs

contains(mobility_modules,serviceframework): SUBDIRS += servicefw servicexmlgen  #SFW API
contains(mobility_modules,publishsubscribe): SUBDIRS += vsexplorer qcrmlgen      #P&S API
