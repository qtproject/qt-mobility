#
# Fix up QT_MOBILITY_SOURCE_TREE if it isn't defined
#
# Symbian MCL builds do not run configure and hence don't have 
# .qmake.cache which set QT_MOBILITY_SOURCE_TREE and 
# QT_MOBILITY_BUILD_TREE. Therefore we need to define a fallback.
# This has the disadvantage that shadow builds are not supported for symbian
# but they are on other platforms.


isEmpty(QT_MOBILITY_SOURCE_TREE):QT_MOBILITY_SOURCE_TREE = $$PWD 
isEmpty(QT_MOBILITY_BUILD_TREE):QT_MOBILITY_BUILD_TREE = $$PWD 

#now include the dynamic config
include($$QT_MOBILITY_BUILD_TREE/config.pri)
