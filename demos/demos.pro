include(../staticconfig.pri)

TEMPLATE = subdirs

#Multimedia demos
contains(mobility_modules,multimedia) {
    SUBDIRS += \
        player
}

#Messaging demos
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo* {
    contains(mobility_modules,messaging) {
        !win32-g++ {
	    SUBDIRS += \
                serviceactions
         }
    }
}

contains(mobility_modules, sensors) {
    SUBDIRS += small_screen_sensors
}

contains(mobility_modules,location) {
    equals(QT_MAJOR_VERSION, 4):lessThan(QT_MINOR_VERSION, 7) {
        contains(mobility_modules,bearer) {
    	    SUBDIRS +=  weatherinfo \
		                lightmaps
        }
    } else {
    	SUBDIRS +=  weatherinfo \
		            lightmaps
    }
}

#Contacts demos
contains(mobility_modules,contacts) {
    contains(mobility_modules,versit):contains(QT_CONFIG, declarative) {
	SUBDIRS += qmlcontacts
    }
}

sources.path = $$QT_MOBILITY_DEMOS
INSTALLS += sources
