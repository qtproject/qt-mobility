!symbian:error(This example is for Symbian packaging purposes only.)

TEMPLATE = app
TARGET = S60Examples

include(../../staticconfig.pri)

TEMPLATE = subdirs

load(data_caging_paths)

#ServiceFramework examples
contains(mobility_modules, serviceframework) {
    SUBDIRS += ../bluetoothtransferplugin \
        ../filemanagerplugin #\
#       ../servicebrowser
    
    executables.sources += \
        serviceframework_bluetoothtransferplugin.dll \
        serviceframework_filemanagerplugin.dll
#        servicebrowser.exe 

    pluginstubs.sources = ../bluetoothtransferplugin/qmakepluginstubs/serviceframework_bluetoothtransferplugin.qtplugin \
        ../filemanagerplugin/qmakepluginstubs/serviceframework_filemanagerplugin.qtplugin
    pluginstubs.path = $${QT_PLUGINS_BASE_DIR} #/resource/qt/plugins

#    reg_resource.sources += \
#         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/servicebrowser_reg.rsc 
#    resource.sources += \
#         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/servicebrowser.rsc
}

#BearerManagement examples
contains(mobility_modules, bearer) {
    SUBDIRS += ../bearermonitor \
        ../../tests/bearerex
    executables.sources += \
        bearermonitor.exe \
        BearerEx.exe

    reg_resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/bearermonitor_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/BearerEx_reg.rsc
    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/bearermonitor.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/BearerEx.rsc
}

#Contacts examples
contains(mobility_modules, contacts) {
    SUBDIRS += ../samplephonebook
    
    executables.sources += \
        samplephonebook.exe

    reg_resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/samplephonebook_reg.rsc
    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/samplephonebook.rsc
}

#Publish and Subscribe examples
contains(mobility_modules, publishsubscribe) {
    SUBDIRS += ../publish-subscribe
    
    executables.sources += \
        publish_subscribe.exe 

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/publish_subscribe_reg.rsc
    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/publish_subscribe.rsc
}

#System Information example
contains(mobility_modules, systeminfo) {
    SUBDIRS += ../sysinfo
    
    executables.sources += \
        sysinfo.exe

    reg_resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/sysinfo_reg.rsc

    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/sysinfo.rsc
}

#Messaging examples
contains(mobility_modules, ) {
    SUBDIRS += ../querymessages\
        ../sendmessage\
        ../serviceactions \
        ../../tests/messagesex
    
    executables.sources += \
        querymessages.exe \
        sendmessage.exe \
        serviceactions.exe \
        messagingex.exe

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/querymessages_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/sendmessage_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/serviceactions_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/messagingex_reg.rsc
    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/querymessages.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/sendmessage.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/serviceactions.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/messagingex.rsc
}


executables.path = /sys/bin
reg_resource.path = $$REG_RESOURCE_IMPORT_DIR
resource.path = $$APP_RESOURCE_DIR

DEPLOYMENT += executables \
    pluginstubs \
    reg_resource \
    resource

mifs.sources = \
    $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C611.mif
mifs.path = $$APP_RESOURCE_DIR


# ensure that dependency to QtMobility sis package is added
CONFIG+=mobility
