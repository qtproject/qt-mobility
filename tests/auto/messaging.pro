include(../../staticconfig.pri)

requires(contains(mobility_modules,messaging))

TEMPLATE = subdirs

contains(qmf_enabled,yes)|wince*|win32|symbian|maemo5|maemo6 {
    !win32-g++:SUBDIRS += \
        qmessagestore \
        qmessagestorefiltercache \
        qmessagestorekeys \
        qmessage \
        qmessageservice \
        # support   # this is a convenience library
}
