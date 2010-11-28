TEMPLATE=subdirs
!wince* {

SUBDIRS += dummydynamicplugin \
           dummydynamiccopyplugin \
           nonlandmarkdynamicplugin \
           unittest   

!symbian += dummystaticplugin
} #disable wince plugin autotests for now
