# In maemo we do not want to run the tests immediately after compilation
# but we create a test package that is run in sandbox environment.
maemo {
	CONFIG -= qttest_p4
	CONFIG += link_pkgconfig

	target.path = $$[QT_INSTALL_PREFIX]/lib/libbearer-test
	target.files = $$[TARGET]
	INSTALLS += target
	DEFINES += MAEMO

	PKGCONFIG += glib-2.0 dbus-glib-1 duivaluespace-1.0 osso-ic conninet
}
