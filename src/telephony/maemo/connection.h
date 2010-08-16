#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore>
#include <QtDBus/QDBusMessage>
#include <qmobilityglobal.h>
#include "maemo/types.h"
#include "maemo/dbus-proxy.h"
#include "maemo/interfaces/cli-connection.h"
#include "maemo/ready-object.h"
#include "qtelephonycallinfo.h"

QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace Tp
{
    class Connection : public StatefulDBusProxy
                        , public ReadyObject
                        , public RefCounted
    {
        Q_OBJECT
        Q_DISABLE_COPY(Connection)
    public:
        static const Feature FeatureCore;

        Connection(const QDBusConnection busconnection, const QString &busName, const QString &objectPath, QTelephonyCallListPrivate* callList);
        ~Connection();
        void channelStatusChanged(Tp::Channel* channel);

    private Q_SLOTS:
        void onSelfHandleChanged(uint selfHandle);
        void onNewChannel(const QDBusObjectPath& objectPath, const QString& channelType, uint handleType, uint handle, bool suppressHandler);
        void onConnectionError(const QString& error, const QVariantMap& details);
        void onStatusChanged(uint status, uint reason);

        // for ConnectionInterfaceRequestsInterface signals
        void onNewChannels(const Tp::ChannelDetailsList& channels);
        void onChannelClosed(const QDBusObjectPath& removed);

    private: 
        Tp::Client::ConnectionInterface* pConnectioninterface;
        Tp::Client::ConnectionInterfaceRequestsInterface* pConnectionInterfaceRequestsInterface;
        QTelephonyCallListPrivate* ptelephonyCallList;
    };
    typedef SharedPtr<Connection> ConnectionPtr;
}
#endif // CONNECTION_H
