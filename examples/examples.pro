include(../staticconfig.pri)

TEMPLATE = subdirs

#ServiceFramework examples
contains(mobility_modules,serviceframework) {
    SUBDIRS += filemanagerplugin \
               bluetoothtransferplugin \
               notesmanagerplugin \
               servicebrowser

    !symbian:SUBDIRS+= sfw-notes
    
    contains(QT_CONFIG, declarative) {
        SUBDIRS += declarative-sfw-dialer

        sources.files += declarative-sfw-notes \
                         declarative-sfw-dialer/declarative-sfw-dialer
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
    !symbian|contains(mobility_modules,bearer) {
    	SUBDIRS += qgeoapiui \
                   mapviewer

    }

    contains(mobility_modules,bearer) {
    	SUBDIRS += flickrdemo
                   #mapviewer (disable for now) 
                   #qgeoapiui
        
        contains(QT_CONFIG, webkit) {
            SUBDIRS += fetchgooglemaps
        }
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
contains(mobility_modules,systeminfo): SUBDIRS += sysinfo

#Multimedia
contains(mobility_modules,multimedia) {
    SUBDIRS += \
        radio \
        slideshow \
        audiorecorder \
        audiodevices \
        audioinput \
        audiooutput \
        videographicsitem \
        videowidget
}


#Messaging examples
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5 {
    contains(mobility_modules,messaging) {
        !win32-g++ {
	    SUBDIRS += \
                querymessages \
                writemessage

            contains(mobility_modules,contacts) {
                SUBDIRS += keepintouch
            }

            # MessagingEx lives in tests for some reason
            maemo5:SUBDIRS += ../tests/messagingex
         }
    }
}

# Sensors API examples
contains(mobility_modules,sensors) {
    SUBDIRS += sensors
}

sources.path = $$QT_MOBILITY_PREFIX/bin
INSTALLS += sources
