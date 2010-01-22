INCLUDEPATH += ipc

PRIVATE_HEADERS += \
        ipc/qslotinvoker_p.h \
        ipc/qsignalintercepter_p.h \
        ipc/qmetaobjectbuilder_p.h \
        ipc/instancemanager_p.h \
        ipc/qservicetyperegister.h \
        ipc/qservicecontrol.h \
        ipc/qservicecontrol_p.h \
        ipc/qservicepackage_p.h \
        ipc/objectendpoint_p.h \
        ipc/ipcendpoint_p.h \
        ipc/proxyobject_p.h


SOURCES += \
        ipc/qslotinvoker.cpp \
        ipc/qsignalintercepter.cpp \
        ipc/qmetaobjectbuilder.cpp \
        ipc/qservicetyperegister.cpp \
        ipc/instancemanager.cpp \
        ipc/qservicecontrol.cpp \
        ipc/qservicepackage.cpp \
        ipc/qservicecontrol_p.cpp \
        ipc/objectendpoint.cpp \
        ipc/ipcendpoint.cpp \
        ipc/proxyobject.cpp

!symbian:QT+=network #for QLocalServer
