include(../../staticconfig.pri)

requires(contains(mobility_modules,systeminfo))

TEMPLATE = subdirs
SUBDIRS += qsysteminfo \
      qsystemdeviceinfo \
      qsystemdisplayinfo \
      qsystemstorageinfo \
      qsystemnetworkinfo \
      qsystemscreensaver \
      qsystembatteryinfo

maemo6: {
      #maemo6 specific autotests
      SUBDIRS += maemo6
}
contains(QT_CONFIG,declarative) {
SUBDIRS += qdeclarativebatteryinfo \
           qdeclarativedeviceinfo \
#           qdeclarativedisplayinfo \
           qdeclarativesysteminfo \
           qdeclarativenetworkinfo \
           qdeclarativescreensaver \
#           qdeclarativestorageinfo
}
