TEMPLATE=subdirs
CONFIG+=ordered
!wince* {
!symbian:SUBDIRS+= dummystaticplugin
SUBDIRS += dummydynamicplugin \
           dummydynamiccopyplugin \
           nonlandmarkdynamicplugin \
           unittest   
} #disable wince plugin autotests for now
