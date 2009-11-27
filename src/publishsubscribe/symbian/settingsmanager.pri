HEADERS += xqsettingskey.h \
    xqsettingsmanager.h \
    xqsettingsmanager_p.h \
    settingshandlerbase.h \
    ccentralrepositoryhandler.h \
    cpublishandsubscribehandler.h \
    cenrepmonitor.h \
    pubsubmonitor.h \
    xqcentralrepositoryutils.h \
    xqcentralrepositoryutils_p.h \
    xqpublishandsubscribeutils.h \
    xqpublishandsubscribeutils_p.h \
    xqcentralrepositorysearchcriteria.h \
    xqpublishandsubscribesecuritypolicy.h \

SOURCES += xqsettingskey.cpp \
    xqsettingsmanager.cpp \
    xqsettingsmanager_p.cpp \
    settingshandlerbase.cpp \
    ccentralrepositoryhandler.cpp \
    cpublishandsubscribehandler.cpp \
    cenrepmonitor.cpp \
    pubsubmonitor.cpp \
    xqcentralrepositoryutils.cpp \
    xqcentralrepositoryutils_p.cpp \
    xqpublishandsubscribeutils.cpp \
    xqpublishandsubscribeutils_p.cpp \
    xqcentralrepositorysearchcriteria.cpp \
    xqpublishandsubscribesecuritypolicy.cpp \

symbian:LIBS += -lcentralrepository \
    -lcommonengine
