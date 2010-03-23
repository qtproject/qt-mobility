######################################################################
#
# Mobility API project - Symbian backends
#
######################################################################

TEMPLATE = subdirs

#includes here so that all defines are added here also
include(mediaplayer/mediaplayer_s60.pri)
include(radio/radio.pri)
include(audiosource/audiosource_s60.pri)

symbian:SUBDIRS += ecam openmaxal mmf 


