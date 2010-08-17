include(../staticconfig.pri)

TEMPLATE = subdirs
SUBDIRS += auto benchmarks

contains(mobility_modules,serviceframework): SUBDIRS += testservice2 sampleserviceplugin sampleserviceplugin2
contains(mobility_modules,bearer) {
    symbian:SUBDIRS += bearerex
}

symbian {
    contains(mobility_modules,messaging): SUBDIRS += messagingex
    contains(mobility_modules,publishsubscribe): SUBDIRS += publishsubscribeex
}
