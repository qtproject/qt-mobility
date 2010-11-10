include(../../staticconfig.pri)

requires(contains(mobility_modules,systeminfo))

TEMPLATE = subdirs
SUBDIRS += qsysteminfo \
      qsystemdeviceinfo \
      qsystemdisplayinfo \
      qsystemstorageinfo \
      qsystemnetworkinfo \
      qsystemscreensaver

maemo6: {
      #maemo6 spesific autotests
      SUBDIRS += maemo6
}
