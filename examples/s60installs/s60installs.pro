TEMPLATE = app
TARGET = S60Examples

include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs

#ServiceFramework examples
SUBDIRS += ../filemanagerplugin \
    ../bluetoothtransferplugin \
    ../servicebrowser

#BearerManagement examples
SUBDIRS += ../bearermonitor \
    ../../tests/bearerex

#Contacts examples
SUBDIRS += ../samplephonebook

#Publish and Subscribe examples
SUBDIRS += ../publish-subscribe

#System Information example
SUBDIRS += ../sysinfo

#Messaging examples
SUBDIRS += ../querymessages\
    ../sendmessage\
    ../serviceactions \
    ../../tests/bearerex
    
symbian {

    load(data_caging_paths)

    executables.sources = \
    bearermonitor.exe \
    BearerEx.exe \
    serviceframework_bluetoothtransferplugin.dll \
    serviceframework_filemanagerplugin.dll \
    servicebrowser.exe \
    publish_subscribe.exe \
    querymessages.exe \
    sendmessage.exe \
    serviceactions.exe \
    samplephonebook.exe \
    sysinfo.exe
    
    xml.path = $$DESTDIR/xmldata
    xml.files = bluetoothtransferservice.xml \
        filemanagerservice.xml
    xml.CONFIG = no_link no_dependencies explicit_dependencies no_build combine ignore_no_exist no_clean
    INSTALLS += xml

    executables.path = /sys/bin
    DEPLOYMENT += executables \
        reg_resource \
        resource

    reg_resource.sources = \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/bearermonitor_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/BearerEx_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/servicebrowser_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/publish_subscribe_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/querymessages_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/sendmessage_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/serviceactions_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/samplephonebook_reg.rsc \
         $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/sysinfo_reg.rsc

    reg_resource.path = $$REG_RESOURCE_IMPORT_DIR

    resource.sources = \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/bearermonitor.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/BearerEx.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/servicebrowser.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/publish_subscribe.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/querymessages.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/sendmessage.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/serviceactions.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/samplephonebook.rsc \
         $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/sysinfo.rsc

    resource.path = $$APP_RESOURCE_DIR

    mifs.sources = \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C611.mif
    mifs.path = $$APP_RESOURCE_DIR

    #TARGET.CAPABILITY = ALL -TCB
}
