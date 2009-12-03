TEMPLATE = subdirs
SUBDIRS = exampleinstaller

symbian {
    load(data_caging_paths)
    xmlFiles.sources = ../filemanagerplugin/filemanagerservice.xml \ 
                       ../bluetoothtransferplugin/bluetoothtransferservice.xml 
    xmlFiles.path = c:$$APP_RESOURCE_DIR/xmldata

    plugins.sources = serviceframework_filemanagerplugin.dll \ 
                        serviceframework_bluetoothtransferplugin.dll
    plugins.path = $$QT_PLUGINS_BASE_DIR    
	
    installer.pkg_postrules += "\"$${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/exampleinstaller.exe\" \  
                                         - \"!:\sys\bin\exampleinstaller.exe\",FR,RB,RW"
    
    DEPLOYMENT += xmlFiles \
                  plugins \
                  installer
}
