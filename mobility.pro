######################################################################
#
# Mobility API project
#
######################################################################

TEMPLATE = subdirs
TARGET = 

CONFIG += ordered

include(common.pri)

# Might need to include common.pri or config.pri

SUBDIRS += contacts plugins tests examples
##SUBDIRS += contacts examples
