TEMPLATE = app
TARGET = tst_performance
QT += testlib
CONFIG += qtestlib
include(../tsrc.pri)

symbian:
{
    INCLUDEPATH += .\inc
    INCLUDEPATH += $$SYMBIAN_PATHS
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    SOURCES += tst_performance.cpp

    CONFIG += mobility
    MOBILITY = contacts

    TARGET.CAPABILITY = ReadUserData \
                        WriteUserData

    # Increase max heap size on the HW to 128MB, we are potentially
    # handling a lot of contact data
    TARGET.EPOCHEAPSIZE = 0x200000 0x4000000

    # On the emulator environment the executable cannot be started with
    # 128MB heap size
#	heapsize= \
#	    "$${LITERAL_HASH}ifdef WINSCW" \
#	    "EPOCHEAPSIZE 0x200000 0x4000000" \
#	    "$${LITERAL_HASH}else" \
#	    "EPOCHEAPSIZE 0x200000 0x8000000" \
#	    "$${LITERAL_HASH}endif"
#	MMP_RULES += heapsize

    LIBS += -lqtcontacts \
            -lqtversit
}
