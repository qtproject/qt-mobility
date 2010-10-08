include(../staticconfig.pri)

TEMPLATE = subdirs

#ServiceFramework examples
contains(mobility_modules,serviceframework) {
    SUBDIRS += filemanagerplugin \
            bluetoothtransferplugin \
            notesmanagerplugin \
            servicebrowser \
	    sfwecho

    #These examples do not work on Symbian yet
    !symbian:SUBDIRS+= sfw-notes
    
    contains(QT_CONFIG, declarative) {
        SUBDIRS += declarative-sfw-dialer declarative-sfw-notes
    }
}


#BearerManagement examples
contains(mobility_modules,bearer) {
    SUBDIRS += bearermonitor bearercloud
}

#Location examples
contains(mobility_modules,location) {
    SUBDIRS += logfilepositionsource \
               satellitedialog 

    !wince* {
        SUBDIRS += landmarkbrowser
    }

#    contains(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 6) {
#    	SUBDIRS += geoservicedemo \
#                    mapviewer
#
#    } else {
#        contains(mobility_modules,bearer) {
#    	    SUBDIRS += geoservicedemo \
#                       mapviewer
#        }
#    }

    contains(mobility_modules,bearer) {
    	SUBDIRS += flickrdemo \
                    geoservicedemo \
                    mapviewer
      }  
    contains(QT_CONFIG, declarative) {
        SUBDIRS += declarative-location
    }
}

#Contacts examples
contains(mobility_modules,contacts) {
    SUBDIRS += samplephonebook
}

#Publish and Subscribe examples
contains(mobility_modules,publishsubscribe) {
    !wince*{
        SUBDIRS += publish-subscribe
        contains(QT_CONFIG, declarative) {
            SUBDIRS += battery-charge

            sources.files += battery-charge/battery-subscriber
        }
    }
}

#System Information
contains(mobility_modules,systeminfo) {
 SUBDIRS += sysinfo
   contains(QT_CONFIG, declarative) {
        SUBDIRS += declarative-systeminfo
   }
}

#Multimedia
contains(mobility_modules,multimedia) {
    SUBDIRS += \
        radio \
        camera \
        slideshow \
        audiorecorder \
        audiodevices \
        audioinput \
        audiooutput \
        videographicsitem \
        videowidget

    contains(QT_CONFIG, declarative) {
        SUBDIRS += declarative-camera
    }
}


#Messaging examples
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5|maemo6 {
    contains(mobility_modules,messaging) {
        !win32-g++ {
	    SUBDIRS += \
                querymessages \
                writemessage

            contains(mobility_modules,contacts) {
                SUBDIRS += keepintouch
            }

            # MessagingEx lives in tests for some reason
            maemo5|maemo6:SUBDIRS += ../tests/messagingex
         }
    }
}

# Sensors API examples
contains(mobility_modules,sensors) {
    SUBDIRS += sensors
}

contains(mobility_modules,gallery) {
    SUBDIRS += \
        documentproperties \
        mediabrowser

    contains(QT_CONFIG, declarative) {
        SUBDIRS += \
                declarative-music-browser
    }
}

# Organizer API examples
contains(mobility_modules, organizer) {
    SUBDIRS += calendardemo
	SUBDIRS += todo
}

# Feedback API examples
contains(mobility_modules, feedback) {
    SUBDIRS += hapticsplayer hapticsquare # this not a good UI for mobile screens at the moment
}

sources.path = $$QT_MOBILITY_EXAMPLES

INSTALLS += sources

