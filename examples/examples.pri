# We want to create a separate package with examples so we need to install
# the result binaries
maemo {
	CONFIG += link_pkgconfig

	target.path = $$[QT_INSTALL_PREFIX]/bin
	target.files = $$[TARGET]
	INSTALLS += target
	DEFINES += MAEMO

	PKGCONFIG += glib-2.0 dbus-glib-1 duivaluespace-1.0 osso-ic conninet
}
