SOURCES=sys_calendar.qtt
CONFIG+=systemtest
maemo5|maemo6 {
    target.path = /usr/local/bin
    INSTALLS += target
}
