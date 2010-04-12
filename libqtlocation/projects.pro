#####################################################################
# Main projectfile
#####################################################################

CONFIG += ordered 
TEMPLATE = subdirs

# include (shared.pri) LIST= something...

SUBDIRS = src tests

QMAKE_DISTCLEAN +=  build-stamp configure-stamp
