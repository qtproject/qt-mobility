TEMPLATE = subdirs

SUBDIRS = \
        player \
        cameracapture \
        slideshow \
        streamplayer \
        audiorecorder
        
symbian: {
         TARGET.CAPABILITY = MultimediaDD WriteUserData ReadUserData UserEnvironment
}
