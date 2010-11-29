TEMPLATE=subdirs
!wince* {

SUBDIRS += dummydynamicplugin \
           dummydynamiccopyplugin \
           nonlandmarkdynamicplugin \
           unittest   

!symbian:SUBDIRS+= dummystaticplugin
} #disable wince plugin autotests for now
