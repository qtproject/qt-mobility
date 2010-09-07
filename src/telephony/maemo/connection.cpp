/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "maemo/constants.h"
#include "maemo/connection.h"
#include "maemo/channel.h"
#include <QtCore/QDebug>
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/qtelephonycallinfo_maemo_p.h"

namespace DBus
{
    using namespace Interfaces;

    const Feature Connection::FeatureCore = Feature(QLatin1String(Connection::staticMetaObject.className()), 0, true);

    Connection::Connection(const QDBusConnection busconnection, const QString &busName, const QString &objectPath, QTelephonyCallListPrivate* callList)
        : StatefullDBusProxy(busconnection, busName, objectPath)
        , ReadyObject(this, FeatureCore)
        , ptelephonyCallList(callList)
    {
        qDebug() << "Connection::Connection(...)";

        pIConnection = new DBus::Interfaces::IConnection(busconnection, busName, objectPath);
        connect(pIConnection, SIGNAL(SelfHandleChanged(uint)), SLOT(onSelfHandleChanged(uint)));
        connect(pIConnection, SIGNAL(NewChannel(QDBusObjectPath,QString,uint,uint,bool)), SLOT(onNewChannel(QDBusObjectPath,QString,uint,uint,bool)));
        connect(pIConnection, SIGNAL(ConnectionError(QString,QVariantMap)), SLOT(onConnectionError(QString,QVariantMap)));
        connect(pIConnection, SIGNAL(StatusChanged(uint,uint)), SLOT(onStatusChanged(uint,uint)));

        pIConnectionRequests = new DBus::Interfaces::IConnectionRequests(busconnection, this->busName(), this->objectPath());
        connect(pIConnectionRequests, SIGNAL(NewChannels(DBus::Interfaces::ChannelDetailsList)), SLOT(onNewChannels(DBus::Interfaces::ChannelDetailsList)));
        connect(pIConnectionRequests, SIGNAL(ChannelClosed(QDBusObjectPath)), SLOT(onChannelClosed(QDBusObjectPath)));

        readCurrentChannels();
    }

    Connection::~Connection()
    {
        qDebug() << "Connection::~Connection()";
        if(pIConnection){
            disconnect(pIConnection, SIGNAL(SelfHandleChanged(uint)), this, SLOT(onSelfHandleChanged(uint)));
            disconnect(pIConnection, SIGNAL(NewChannel(QDBusObjectPath,QString,uint,uint,bool)), this, SLOT(onNewChannel(QDBusObjectPath,QString,uint,uint,bool)));
            disconnect(pIConnection, SIGNAL(ConnectionError(QString,QVariantMap)), this, SLOT(onConnectionError(QString,QVariantMap)));
            disconnect(pIConnection, SIGNAL(StatusChanged(uint,uint)), this, SLOT(onStatusChanged(uint,uint)));
            delete pIConnection;
        }
        if(pIConnectionRequests){
            disconnect(pIConnectionRequests, SIGNAL(NewChannels(DBus::Interfaces::ChannelDetailsList)), this, SLOT(onNewChannels(DBus::Interfaces::ChannelDetailsList)));
            disconnect(pIConnectionRequests, SIGNAL(ChannelClosed(QDBusObjectPath)), this, SLOT(onChannelClosed(QDBusObjectPath)));
            delete pIConnectionRequests;
        }
    }

    void Connection::readCurrentChannels()
    {
        if(pIConnection){
            QDBusPendingReply<DBus::Interfaces::ChannelInfoList> dbrlistchannels = pIConnection->ListChannels();
            dbrlistchannels.waitForFinished();
            DBus::Interfaces::ChannelInfoList channellist = dbrlistchannels.value();
            foreach(const DBus::Interfaces::ChannelInfo& chi, channellist){
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

    //for IConnectionRequests signals
    void Connection::onNewChannels(const DBus::Interfaces::ChannelDetailsList& channels)
    {
        qDebug() << "Connection::IConnectionRequests::onNewChannels(...)";
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
        qDebug() << "Connection::IConnectionRequests::onChannelClosed(...)";
    }

    //own slots
    void Connection::channelStatusChanged(DBus::Channel *channel)
    {
        ptelephonyCallList->channelStatusChanged(ChannelPtr(channel));
    }
}
