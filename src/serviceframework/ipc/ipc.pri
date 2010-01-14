INCLUDEPATH += ipc

PRIVATE_HEADERS += \
        ipc/qslotinvoker_p.h \
        ipc/qsignalintercepter_p.h \
        ipc/qmetaobjectbuilder_p.h \
        ipc/instancemanager_p.h \
        ipc/qservicetyperegister.h


SOURCES += \
        ipc/qslotinvoker.cpp \
        ipc/qsignalintercepter.cpp \
        ipc/qmetaobjectbuilder.cpp \
        ipc/qservicetyperegister.cpp \
        ipc/instancemanager.cpp
