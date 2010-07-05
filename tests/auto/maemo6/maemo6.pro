TEMPLATE = subdirs

include(../../../staticconfig.pri)

contains(mobility_modules,systeminfo) {
	SUBDIRS += qsystemnetworkinfo_maemo
}
