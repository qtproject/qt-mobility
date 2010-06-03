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
    INCLUDEPATH += \epoc32\include\app
	INCLUDEPATH += \epoc32\include\platform\app
    SOURCES  += tst_simcm.cpp
    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData
    LIBS += -lqtcontacts
}
