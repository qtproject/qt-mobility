HEADERS += xqsettingskey_p.h \
    xqsettingsmanager.h \
    xqsettingsmanager_p.h \
    settingshandlerbase_p.h \
    ccentralrepositoryhandler_p.h \
    cpublishandsubscribehandler_p.h \
    cenrepmonitor_p.h \
    pubsubmonitor_p.h \
    xqcentralrepositoryutils_p.h \
    xqcentralrepositoryutils_p.h \
    xqpublishandsubscribeutils.h \
    xqpublishandsubscribeutils_p.h \
    xqcentralrepositorysearchcriteria_p.h \
    xqpublishandsubscribesecuritypolicy_p.h \

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
