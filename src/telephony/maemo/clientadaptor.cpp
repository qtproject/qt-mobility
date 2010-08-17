/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2009 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include "clientadaptor.h"
#include "types.h"

namespace Tp
{
    ClientAdaptor::ClientAdaptor(const QStringList &interfaces, QObject *parent)
        : QDBusAbstractAdaptor(parent),
          mInterfaces(interfaces)
    {
    }

    ClientAdaptor::~ClientAdaptor()
    {
    }

    ClientObserverAdaptor::ClientObserverAdaptor(const QDBusConnection &bus,
            AbstractClientObserver *client,
            QObject *parent)
        : QDBusAbstractAdaptor(parent),
          mBus(bus),
          mClient(client)
    {
    }

    ClientObserverAdaptor::~ClientObserverAdaptor()
    {
    }

    void ClientObserverAdaptor::ObserveChannels(const QDBusObjectPath &accountPath,
            const QDBusObjectPath &connectionPath,
            const Tp::ChannelDetailsList &channelDetailsList,
            const QDBusObjectPath &dispatchOperationPath,
            const Tp::ObjectPathList &requestsSatisfied,
            const QVariantMap &observerInfo,
            const QDBusMessage &message)
    {
        qDebug() << "ObserveChannels: account:" << accountPath.path() <<
            ", connection:" << connectionPath.path();

        mClient->observeChannels(accountPath, connectionPath, channelDetailsList
                                 ,dispatchOperationPath, requestsSatisfied, observerInfo, message);
    }

} // Tp
