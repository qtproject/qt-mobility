INCLUDEPATH += ipc

symbian {
    PRIVATE_HEADERS += ipc/qremoteservicecontrol_s60_p.h
    SOURCES += ipc/qremoteservicecontrol_s60.cpp

} else {
    PRIVATE_HEADERS += ipc/qremoteservicecontrol_p.h
    SOURCES += ipc/qremoteservicecontrol_p.cpp
}

PRIVATE_HEADERS += \
        ipc/qslotinvoker_p.h \
        ipc/qsignalintercepter_p.h \
        ipc/qmetaobjectbuilder_p.h \
        ipc/instancemanager_p.h \
        ipc/qservicepackage_p.h \
        ipc/objectendpoint_p.h \
        ipc/ipcendpoint_p.h \
        ipc/proxyobject_p.h

SOURCES += \
        ipc/qslotinvoker.cpp \
        ipc/qsignalintercepter.cpp \
        ipc/qmetaobjectbuilder.cpp \
        ipc/instancemanager.cpp \
        ipc/qservicepackage.cpp \
        ipc/objectendpoint.cpp \
        ipc/ipcendpoint.cpp \
        ipc/proxyobject.cpp

!symbian:QT+=network #for QLocalServer

