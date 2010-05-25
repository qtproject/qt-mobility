TEMPLATE = lib
TARGET = RfsServiceFrameworkPlugin
DEPENDPATH += inc src

HEADERS += inc/rfsserviceframeworkplugin.h \
           inc/rfsserviceframeworkplugin.hrh

SOURCES += src/rfsserviceframeworkplugin.cpp \
           src/rfsserviceframeworkproxy.cpp
           
LIBS += -leuser \
	-lecom \
	-lefsrv \
	-lplatformenv \
	-lbafl \
	-lestor

TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_ECOM_PLUGIN
TARGET.UID2 = 0x10009D8D
TARGET.UID3 = 0xE000C8D3

sysInclude = "SYSTEMINCLUDE /epoc32/include/ecom"
mwInclude = "MW_LAYER_SYSTEMINCLUDE"

resource = \
  "START RESOURCE ../data/E000C8D3.rss" \
  "TARGETPATH ECOM_RESOURCE_DIR" \
  "$${LITERAL_HASH}ifdef SYMBIAN_SECURE_ECOM" \
  "TARGET rfsserviceframeworkplugin.rsc" \
  "$${LITERAL_HASH}endif" \
  "END"

#    defBlock = \
#      "$${LITERAL_HASH}if defined(MARM)" \
#      "DEFFILE  eabi/rfsserviceframeworklugin.def" \
#      "$${LITERAL_HASH}else" \
#      "DEFFILE  bwins/rfsserviceframeworklugin.def" \
#      "$${LITERAL_HASH}endif"

#    MMP_RULES += defBlock

MMP_RULES += sysInclude mwInclude resource

#deploy factory reset script
rfsScript.sources = data/sfwrfs.txt
rfsScript.path = z:\\private\\E000C8D3
DEPLOYMENT += rfsScript
