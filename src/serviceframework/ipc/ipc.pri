INCLUDEPATH += ipc

symbian {
    PRIVATE_HEADERS += ipc/qremoteservicecontrol_s60_p.h \
                       ipc/objectendpoint_p.h \
                       ipc/proxyobject_p.h
    
    SOURCES += ipc/qremoteservicecontrol_s60.cpp \    
               ipc/objectendpoint.cpp \
               ipc/proxyobject.cpp

} else {
    contains(QT_CONFIG,dbus) {
        QT += dbus network
        
        PRIVATE_HEADERS += ipc/qremoteservicecontrol_dbus_p.h \
                           ipc/objectendpoint_dbus_p.h \
                           ipc/qservicemetaobject_dbus_p.h
        SOURCES += ipc/qremoteservicecontrol_dbus_p.cpp \
                   ipc/objectendpoint_dbus.cpp \
                   ipc/qservicemetaobject_dbus.cpp

    } else {
        QT += network

        PRIVATE_HEADERS += ipc/qremoteservicecontrol_p.h \
                           ipc/objectendpoint_p.h
        SOURCES += ipc/qremoteservicecontrol_p.cpp \
                   ipc/objectendpoint.cpp
    }
}

PRIVATE_HEADERS += \
        ipc/qslotinvoker_p.h \
        ipc/qsignalintercepter_p.h \
        ipc/qmetaobjectbuilder_p.h \
        ipc/instancemanager_p.h \
        ipc/qservicepackage_p.h \
        ipc/proxyobject_p.h \
        ipc/ipcendpoint_p.h

SOURCES += \
        ipc/qslotinvoker.cpp \
        ipc/qsignalintercepter.cpp \
        ipc/qmetaobjectbuilder.cpp \
        ipc/instancemanager.cpp \
        ipc/qservicepackage.cpp \
        ipc/proxyobject.cpp \
        ipc/ipcendpoint.cpp
