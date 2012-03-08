TEMPLATE = app
TARGET = sfwtest
CONFIG += mobility
MOBILITY = serviceframework
QT += core
QT -= gui
HEADERS += sfwtestservice.h
SOURCES += sfwtestservice.cpp \
    main.cpp

symbian: { 
    TARGET.UID3 = 0xe3260ac0
    TARGET.VID = VID_DEFAULT
  	SYMBIAN_PLATFORMS = WINSCW ARMV5
    addFiles.sources = sfwtestservice.xml
  	addFiles.path = /resource/apps/xmldata/
  	DEPLOYMENT += addFiles
    
}
