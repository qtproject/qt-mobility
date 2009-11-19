TEMPLATE = subdirs
#remove service xml gen for now -> breaks the build
#SUBDIRS += servicefw servicexmlgen  #SFW API
SUBDIRS += servicefw  #SFW API
SUBDIRS += vsexplorer qcrmlgen      #P&S API
