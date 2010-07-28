!symbian:error(This example is for Symbian packaging purposes only.)

TEMPLATE = app
TARGET = QtMobilityS60Examples

include(../../staticconfig.pri)

TEMPLATE = subdirs

load(data_caging_paths)

#BearerManagement examples
contains(mobility_modules, bearer) {
    SUBDIRS +=  ../bearermonitor \
                ../bearercloud
    executables.sources += \
        bearermonitor.exe \
        bearercloud.exe

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/bearermonitor_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/bearercloud_reg.rsc
    resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/bearermonitor.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/bearercloud.rsc
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

#Location examples
contains(mobility_modules, location) {
    SUBDIRS +=  ../logfilepositionsource \
        ../satellitedialog

    executables.sources += \
        logfilepositionsource.exe \
        satellitedialog.exe

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/logfilepositionsource_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/satellitedialog_reg.rsc
    resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/logfilepositionsource.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/satellitedialog.rsc
}

#Messaging examples
contains(mobility_modules, messaging) {
    SUBDIRS += ../querymessages \
        ../serviceactions
    
    executables.sources += \
        querymessages.exe \
        serviceactions.exe

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/querymessages_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/serviceactions_reg.rsc
    resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/querymessages.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/serviceactions.rsc
}

#Multimedia examples
contains(mobility_modules, multimedia) {
    SUBDIRS +=  ../player \
                ../audiorecorder
    
    executables.sources += \
        player.exe \
        audiorecorder.exe

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/player_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/audiorecorder_reg.rsc
    resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/player.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/audiorecorder.rsc
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

#Sensors examples
contains(mobility_modules, sensors) {
    SUBDIRS += ../sensors/small_screen_sensors
    executables.sources += \
        smallsensors.exe

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/smallsensors_reg.rsc

    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/smallsensors.rsc
}

#ServiceFramework examples
contains(mobility_modules, serviceframework) {
    SUBDIRS +=  ../bluetoothtransferplugin \
                ../filemanagerplugin \
                ../servicebrowser
    
    executables.sources += \
        serviceframework_bluetoothtransferplugin.dll \
        serviceframework_filemanagerplugin.dll \
        servicebrowser.exe 

    pluginstubs.sources =   ../bluetoothtransferplugin/qmakepluginstubs/serviceframework_bluetoothtransferplugin.qtplugin \
                            ../filemanagerplugin/qmakepluginstubs/serviceframework_filemanagerplugin.qtplugin
    pluginstubs.path = $${QT_PLUGINS_BASE_DIR} #/resource/qt/plugins

    reg_resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/servicebrowser_reg.rsc 
    resource.sources += \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/servicebrowser.rsc
}

#System Information example
contains(mobility_modules, systeminfo) {
    SUBDIRS += ../qsysinfo
    
    executables.sources += \
        qsysinfo.exe

    reg_resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/qsysinfo_reg.rsc

    resource.sources += \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/qsysinfo.rsc
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
