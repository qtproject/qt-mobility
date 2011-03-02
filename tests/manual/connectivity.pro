include(../../staticconfig.pri)

requires(contains(mobility_modules,connectivity))

TEMPLATE = subdirs

SUBDIRS += \
    qllcpsocket \
    qllcpserver

