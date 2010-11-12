do_nothing : ;

TMPROOT:=$(subst \,/,$(EPOCROOT))
EPOCROOT:=$(patsubst %/,%,$(TMPROOT))/

SFWTOOL=$(EPOCROOT)epoc32/tools/servicedbgen

#choose target database to deal with
ifeq ($(PLATFORM), WINSCW)
	QTSERVICE_TARGET:=-twinscw
else
ifeq ($(PLATFORM), ARMV5)
QTSERVICE_TARGET:=-tarmv5
else
	QTSERVICE_TARGET:=-tall
endif
endif

#Service descriptors are deployed to SFW private folder. As the builder may get invoked from elsewhere,
#we cannot use the path specified in the QTSERVICE_DESCRIPTOR option, so use the default path
DES_PATH=$(EPOCROOT)epoc32/data/z/private/2002AC7F/des/$(QTSERVICE_DESCRIPTOR)

MAKMAKE: 
	@$(SFWTOOL) $(QTSERVICE_TARGET) $(QTSERVICE_INITIALIZE) add $(DES_PATH)

CLEAN: 
	@$(SFWTOOL) $(QTSERVICE_TARGET) $(QTSERVICE_INITIALIZE) remove $(DES_PATH)

BLD SAVESPACE FREEZE LIB CLEANLIB RESOURCE RELEASABLES FINAL: do_nothing
