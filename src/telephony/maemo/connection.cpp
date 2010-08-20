#include "maemo/cli-constants.h"
#include "maemo/connection.h"
#include "maemo/channel.h"
#include <QtCore/QDebug>
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/qtelephonycallinfo_maemo_p.h"

namespace Tp
{
    const Feature Connection::FeatureCore = Feature(QLatin1String(Connection::staticMetaObject.className()), 0, true);

    Connection::Connection(const QDBusConnection busconnection, const QString &busName, const QString &objectPath, QTelephonyCallListPrivate* callList)
        : StatefulDBusProxy(busconnection, busName, objectPath)
        , ReadyObject(this, FeatureCore)
        , ptelephonyCallList(callList)
    {
        qDebug() << "Connection::Connection(...)";
        qDebug() << "- QDBusConnection base service: " << busconnection.baseService();
        qDebug() << "- QDBusConnection name: " << busconnection.name();
        qDebug() << "- objectPath: " << objectPath;
        qDebug() << "- DBusProxy.busName: " << this->busName();
        qDebug() << "- DBusProxy.objectPath: " << this->objectPath();

        pConnectioninterface = new Tp::Client::ConnectionInterface(busconnection, busName, objectPath);
        connect(pConnectioninterface, SIGNAL(SelfHandleChanged(uint)), SLOT(onSelfHandleChanged(uint)));
        connect(pConnectioninterface, SIGNAL(NewChannel(QDBusObjectPath,QString,uint,uint,bool)), SLOT(onNewChannel(QDBusObjectPath,QString,uint,uint,bool)));
        connect(pConnectioninterface, SIGNAL(ConnectionError(QString,QVariantMap)), SLOT(onConnectionError(QString,QVariantMap)));
        connect(pConnectioninterface, SIGNAL(StatusChanged(uint,uint)), SLOT(onStatusChanged(uint,uint)));

        pConnectionInterfaceRequestsInterface = new Tp::Client::ConnectionInterfaceRequestsInterface(busconnection, this->busName(), this->objectPath());
        connect(pConnectionInterfaceRequestsInterface, SIGNAL(NewChannels(Tp::ChannelDetailsList)), SLOT(onNewChannels(Tp::ChannelDetailsList)));
        connect(pConnectionInterfaceRequestsInterface, SIGNAL(ChannelClosed(QDBusObjectPath)), SLOT(onChannelClosed(QDBusObjectPath)));

        readCurrentChannels();
    }

    Connection::~Connection()
    {
        qDebug() << "Connection::~Connection()";
        if(pConnectioninterface){
            disconnect(pConnectioninterface, SIGNAL(SelfHandleChanged(uint)), this, SLOT(onSelfHandleChanged(uint)));
            disconnect(pConnectioninterface, SIGNAL(NewChannel(QDBusObjectPath,QString,uint,uint,bool)), this, SLOT(onNewChannel(QDBusObjectPath,QString,uint,uint,bool)));
            disconnect(pConnectioninterface, SIGNAL(ConnectionError(QString,QVariantMap)), this, SLOT(onConnectionError(QString,QVariantMap)));
            disconnect(pConnectioninterface, SIGNAL(StatusChanged(uint,uint)), this, SLOT(onStatusChanged(uint,uint)));
            delete pConnectioninterface;
        }
        if(pConnectionInterfaceRequestsInterface){
            disconnect(pConnectionInterfaceRequestsInterface, SIGNAL(NewChannels(Tp::ChannelDetailsList)), this, SLOT(onNewChannels(Tp::ChannelDetailsList)));
            disconnect(pConnectionInterfaceRequestsInterface, SIGNAL(ChannelClosed(QDBusObjectPath)), this, SLOT(onChannelClosed(QDBusObjectPath)));
            delete pConnectionInterfaceRequestsInterface;
        }
    }

    void Connection::readCurrentChannels()
    {
        qDebug() << "Connection::readCurrentChannels()";

        if(pConnectioninterface){
            QDBusPendingReply<Tp::ChannelInfoList> dbrlistchannels = pConnectioninterface->ListChannels();
            dbrlistchannels.waitForFinished();
            Tp::ChannelInfoList channellist = dbrlistchannels.value();
            foreach(const Tp::ChannelInfo& chi, channellist){
                qDebug() << "==============================================>";
                QString connectionbusname = PATH2BUS(this->objectPath());
                qDebug() << "- current Channel found";
                qDebug() << "-- Path " << chi.channel.path();
                qDebug() << "-- connectionbusname: " << connectionbusname;
                qDebug() << "-- channelType " << chi.channelType;
                if(Channel::isCall(chi.channelType)){
                    qDebug() << "-- is call";
                    Channel* pchannel = new Channel(QDBusConnection::sessionBus(), connectionbusname,  chi.channel.path(), QVariantMap(), this);
                    ptelephonyCallList->newChannels(ChannelPtr(pchannel));
                }
                else
                    qDebug() << "-- is no call";
                qDebug() << "==============================================<";
            }
        }
    }

    //Slots
    void Connection::onSelfHandleChanged(uint selfHandle)
    {
        qDebug() << "Connection::onSelfHandleChanged(...)";
    }

    void Connection::onNewChannel(const QDBusObjectPath& objectPath, const QString& channelType, uint handleType, uint handle, bool suppressHandler)
    {
        qDebug() << "Connection::onNewChannel(...)";
    }

    void Connection::onConnectionError(const QString& error, const QVariantMap& details)
    {
        qDebug() << "Connection::onConnectionError(...)";
    }

    void Connection::onStatusChanged(uint status, uint reason)
    {
        qDebug() << "Connection::onStatusChanged(...)";
    }

    //for ConnectionInterfaceRequestsInterface signals
    void Connection::onNewChannels(const Tp::ChannelDetailsList& channels)
    {
        qDebug() << "Connection::ConnectionInterfaceRequestsInterface::onNewChannels(...)";
        foreach (const ChannelDetails &channelDetails, channels) {
            QString connectionbusname = PATH2BUS(this->objectPath());
            qDebug() << "- object Path: " << channelDetails.channel.path();
            qDebug() << "- connectionbusname: " << connectionbusname;
            qDebug() << "- check if call";
            qDebug() << "- Creating QTelephonyCallInfoPrivate";

            if( channelDetails.properties.contains(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"))
                && Channel::isCall(channelDetails.properties.value(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType")).toString())){
                Channel* pchannel = new Channel(QDBusConnection::sessionBus(), connectionbusname, channelDetails.channel.path(), channelDetails.properties, this);
                ptelephonyCallList->newChannels(ChannelPtr(pchannel));
            }
        }
    }

    void Connection::onChannelClosed(const QDBusObjectPath& removed)
    {
        qDebug() << "Connection::ConnectionInterfaceRequestsInterface::onChannelClosed(...)";
    }

    void Connection::channelStatusChanged(Tp::Channel *channel)
    {
        qDebug() << "Connection::channelStatusChanged(...)";
        ptelephonyCallList->channelStatusChanged(ChannelPtr(channel));
    }
}
