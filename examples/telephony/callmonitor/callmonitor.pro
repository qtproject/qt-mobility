TEMPLATE = subdirs

linux-*:!maemo* {
	SUBDIRS += linux
}
maemo* {
	SUBDIRS += maemo
}