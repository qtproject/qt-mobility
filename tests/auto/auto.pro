TEMPLATE = subdirs

include(../../staticconfig.pri)

contains(mobility_modules,bearer):          SUBDIRS+=bearer.pro
contains(mobility_modules,contacts):        SUBDIRS+=contacts.pro
contains(mobility_modules,feedback):        SUBDIRS+=feedback.pro
contains(mobility_modules,gallery):         SUBDIRS+=gallery.pro
contains(mobility_modules,location):        SUBDIRS+=location.pro
contains(mobility_modules,messaging):       SUBDIRS+=messaging.pro
contains(mobility_modules,multimedia):      SUBDIRS+=multimedia.pro
contains(mobility_modules,organizer):       SUBDIRS+=organizer.pro
contains(mobility_modules,publishsubscribe):SUBDIRS+=publishsubscribe.pro
contains(mobility_modules,sensors):         SUBDIRS+=sensors.pro
contains(mobility_modules,serviceframework):SUBDIRS+=serviceframework.pro
contains(mobility_modules,systeminfo):      SUBDIRS+=systeminfo.pro
contains(mobility_modules,versit):          SUBDIRS+=versit.pro
contains(mobility_modules,connectivity):    SUBDIRS+=connectivity.pro

# These autotests consist of things such as static code checks
# which require that the autotest is run on the same machine
# doing the build - i.e. cross-compilation is not allowed.
win32|mac|linux-g++* {
    !embedded:!maemo5:!maemo6:SUBDIRS+=host.pro
}
