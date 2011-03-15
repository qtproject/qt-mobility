TEMPLATE = app
TARGET = nfctestserviceprovider 

QT        += core \
    gui 



CONFIG += mobility no_icon
MOBILITY = serviceframework \
             connectivity
  
INCLUDEPATH += ../../../src/connectivity/nfc
INCLUDEPATH += ../common

HEADERS   += nfctestserviceprovider.h
SOURCES   += nfctestserviceprovider_reg.rss \
    main.cpp \
    nfctestserviceprovider.cpp
FORMS	  += nfctestserviceprovider.ui
RESOURCES +=

symbian: {
	TARGET.UID3 = 0xe347d948
	MMP_RULES += DEBUGGABLE_UDEBONLY 
}


wince*|symbian*: {
    addFiles.sources = nfctestserviceprovider.xml
    addFiles.path = xmldata
    addFiles2.sources = nfctestserviceprovider.xml
    addFiles2.path = /private/2002AC7F/import/
    DEPLOYMENT += addFiles addFiles2
}

symbian { 
	TARGET.UID3 = 0xe347d949
    TARGET.CAPABILITY = LocalServices
   }


wince* {
    DEFINES+= TESTDATA_DIR=\\\".\\\"
} else:!symbian {
    DEFINES += TESTDATA_DIR=\\\"$$PWD/\\\"
}

xml.path = $$QT_MOBILITY_EXAMPLES/xmldata
xml.files = nfctestserviceprovider.xml
INSTALLS += xml

