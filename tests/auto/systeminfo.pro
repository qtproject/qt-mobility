include(../../staticconfig.pri)

requires(contains(mobility_modules,systeminfo))

TEMPLATE = subdirs
SUBDIRS += qsysteminfo \
      qsystemdeviceinfo \
      qsystemdisplayinfo \
      qsystemstorageinfo \
      qsystemnetworkinfo \
      qsystemscreensaver \
      qsystemalignedtimer

maemo6: {
      #maemo6 specific autotests
      SUBDIRS += maemo6
}
contains(QT_CONFIG,declarative) {
SUBDIRS += \
           qdeclarativedeviceinfo \
           qdeclarativedisplayinfo \
           qdeclarativesysteminfo \
           qdeclarativenetworkinfo \
           qdeclarativescreensaver \
#           qdeclarativestorageinfo
}

 mac:contains(corewlan_enabled, yes) {
# something is weird about 10.5, it can't find includes.
 SUBDIRS += qsystembatteryinfo \
            qdeclarativebatteryinfo
 }
 !mac: {
SUBDIRS += qsystembatteryinfo \
            qdeclarativebatteryinfo

 }
