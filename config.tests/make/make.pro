
contains(MAKEFILE_GENERATOR, SYMBIAN_ABLD) {
	message(symbian-abld)
} else:contains(MAKEFILE_GENERATOR, SYMBIAN_SBSV2) {
    message(symbian-sbsv2)
} else:contains(MAKEFILE_GENERATOR, MSVC)|contains(MAKEFILE_GENERATOR, MSVC.NET) {
    message(win32-nmake)
} else:contains(MAKEFILE_GENERATOR, MINGW) {
    message(win32-mingw)
}

