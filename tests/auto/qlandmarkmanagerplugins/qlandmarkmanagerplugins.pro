TEMPLATE=subdirs
!wince* {
SUBDIRS += dummystaticplugin \
           dummydynamicplugin \
           dummydynamiccopyplugin \
           nonlandmarkdynamicplugin \
           unittest   
} #disable wince plugin autotests for now
