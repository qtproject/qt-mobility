TEMPLATE=app
TARGET=tst_simcm

QT += testlib
CONFIG += qtestlib

include(../../symbiansim_defines.pri)

symbian: {
    INCLUDEPATH += .\inc
	INCLUDEPATH += \
	        ../../../../../src/global \
	        ../../../../../src/contacts \
	        ../../../../../src/contacts/details \
	        ../../../../../src/contacts/filters \
	        ../../../../../src/contacts/requests
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    SOURCES  += tst_simcm.cpp
    TARGET.CAPABILITY = ALL -TCB
    LIBS += -lqtcontacts
}
