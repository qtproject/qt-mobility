include(../staticconfig.pri)

TEMPLATE = subdirs

#Multimedia demos
contains(mobility_modules,multimedia) {
    SUBDIRS += \
        player
}

#Messaging demos
contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5 {
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
    contains(mobility_modules,bearer) {
    	SUBDIRS += weatherinfo \
		   lightmaps
    }
}

#Contacts demos
contains(mobility_modules,contacts) {
    contains(mobility_modules,versit):contains(QT_CONFIG, declarative) {
        sources.files += qmlcontacts
    }
}
