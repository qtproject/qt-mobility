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
        SUBDIRS += declarative-sfw-notes \
                   declarative-sfw-dialer
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
    contains(mobility_modules,bearer) {
    	SUBDIRS += flickrdemo \
		    weatherinfo \
		    lightmaps
        contains(QT_CONFIG, webkit) {
            SUBDIRS += fetchgooglemaps
        }
    }		
}

#Contacts examples
contains(mobility_modules,contacts) {
    SUBDIRS += samplephonebook
    contains(mobility_modules,versit):contains(QT_CONFIG, declarative) {
        SUBDIRS += qmlcontacts
    }
}

#Publish and Subscribe examples
contains(mobility_modules,publishsubscribe) {
    !wince*{
        SUBDIRS += publish-subscribe
        contains(QT_CONFIG, declarative) {
            SUBDIRS += battery-charge
        }
    }
}

#System Information
contains(mobility_modules,systeminfo): SUBDIRS += sysinfo

#Multimedia
contains(mobility_modules,multimedia) {
    #disabled on Symbian due to missing backend
    SUBDIRS += \
        radio \
        player \
        slideshow \
        audiorecorder
}


#Messaging examples
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5 {
    contains(mobility_modules,messaging) {
        !win32-g++ {
	    SUBDIRS += \
                querymessages \
                writemessage \
                serviceactions

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

# Calendar
SUBDIRS += qmlcalendar

