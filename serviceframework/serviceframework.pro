TEMPLATE = lib
TARGET = QtServiceFramework
QT = core sql

include(../common.pri)

symbian {
    TARGET.CAPABILITY = ALL -TCB
}

 

DEFINES += QT_BUILD_SFW_LIB QT_MAKEDLL


#INCLUDEPATH +=  .       

#DEPENDPATH += servicemetadata \
#              servicedatabase \
#              serviceresolution \
#              servicehandler 

HEADERS +=  qserviceglobal.h \
            servicemetadata_p.h \
            servicedatabase.h \
            databasemanager.h \
            qservicemanager.h \
            qserviceplugininterface.h \
            qservicecontext.h \
            qabstractsecuritysession.h \
            qserviceinterfacedescriptor.h \
            qserviceinterfacedescriptor_p.h \
            qservicefilter.h



SOURCES +=  servicemetadata.cpp \
            servicedatabase.cpp \
            databasemanager.cpp \
            qservicemanager.cpp \
            qserviceplugininterface.cpp \
            qservicecontext.cpp \
            qabstractsecuritysession.cpp \
            qserviceinterfacedescriptor.cpp \
            qservicefilter.cpp

