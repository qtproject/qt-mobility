TEMPLATE = lib
TARGET = QtServiceFramework
QT = core sql

include(../../common.pri)

symbian {
#    TARGET.UID3 = 0x102829E6
#    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = ALL -TCB
#    LIBS += -lsqldb 
#    debug {
#        LIBS += -lflogger
#    }
#    BLD_INF_RULES.prj_exports += \
#        "servicedatabase.h          ..\inc\servicedatabase.h" \
#        "servicedatabasedefines.h   ..\inc\servicedatabasedefines.h"
}

 

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL


INCLUDEPATH +=  ../inc \        # for qserviceglobal.h
                servicemetadata \
                servicedatabase \
                serviceresolution \
                servicehandler \
                ../../security/inc \ #for sec manager includes

DEPENDPATH += servicemetadata \
              servicedatabase \
              serviceresolution \
              servicehandler 

HEADERS +=  ../inc/qserviceglobal.h \
            servicemetadata/servicemetadata_p.h \
            servicedatabase/servicedatabase.h \
            servicehandler/qservicemanager.h \
            servicehandler/qserviceplugininterface.h \
            servicehandler/qservicecontext.h \
            servicehandler/qabstractsecuritysession.h \
            serviceresolution/serviceinfo_p.h \
            serviceresolution/qserviceinterfacedescriptor.h \
            serviceresolution/qserviceinterfacedescriptor_p.h \
            serviceresolution/qservicefilter.h



SOURCES +=  servicemetadata/servicemetadata.cpp \
            servicedatabase/servicedatabase.cpp \
            servicehandler/qservicemanager.cpp \
            servicehandler/qserviceplugininterface.cpp \
            servicehandler/qservicecontext.cpp \
            servicehandler/qabstractsecuritysession.cpp \
            serviceresolution/serviceinfo.cpp \
            serviceresolution/qserviceinterfacedescriptor.cpp \
            serviceresolution/qservicefilter.cpp

