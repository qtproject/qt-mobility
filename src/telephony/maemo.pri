# #####################################################################
# Telephony Mobility API
# #####################################################################

HEADERS +=  maemo\qtelephonycalllist_maemo_p.h \
            maemo\qtelephonycallinfo_maemo_p.h \
            maemo\abstractclient.h \
            maemo\clientregistrar.h \
            maemo\clientadaptor.h \
            maemo\shared-ptr.h \
            maemo\types.h \
            maemo\types-internal.h \
            maemo\dbus-proxy.h \
            maemo\ready-object.h \
            maemo\feature.h \
            maemo\readiness-helper.h \
            maemo\pending-ready.h \
            maemo\pending-operation.h \
            maemo\simple-pending-operations.h \
            maemo\connection.h \
            maemo\channel.h \
            maemo\interfaces\cli-types.h \
            maemo\interfaces\cli-account-manager.h \
            maemo\interfaces\cli-account.h \
            maemo\interfaces\cli-channel-dispatch-operation.h \
            maemo\interfaces\cli-channel-dispatcher.h \
            maemo\interfaces\cli-channel-request.h \
            maemo\interfaces\cli-channel.h \
            maemo\interfaces\cli-client.h \
            maemo\interfaces\cli-connection-manager.h \
            maemo\interfaces\cli-connection.h \
            maemo\interfaces\abstract-interface.h

SOURCES +=  maemo\qtelephonycalllist_maemo.cpp \
            maemo\qtelephonycallinfo_maemo_p.cpp \
            maemo\abstractclient.cpp \
            maemo\clientregistrar.cpp \
            maemo\types.cpp \
            maemo\clientadaptor.cpp \
            maemo\dbus-proxy.cpp \
            maemo\ready-object.cpp \
            maemo\feature.cpp \
            maemo\readiness-helper.cpp \
            maemo\pending-ready.cpp \
            maemo\pending-operation.cpp \
            maemo\connection.cpp \
            maemo\channel.cpp \
            maemo\interfaces\cli-types.cpp \
            maemo\interfaces\cli-account-body.cpp \
            maemo\interfaces\cli-account-manager-body.cpp \
            maemo\interfaces\cli-channel-body.cpp \
            maemo\interfaces\cli-channel-dispatcher-body.cpp \
            maemo\interfaces\cli-channel-dispatch-operation-body.cpp \
            maemo\interfaces\cli-channel-request-body.cpp \
            maemo\interfaces\cli-client-body.cpp \
            maemo\interfaces\cli-connection-body.cpp \
            maemo\interfaces\cli-connection-manager-body.cpp \
            maemo\interfaces\abstract-interface.cpp

