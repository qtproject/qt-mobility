/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbluetoothdevicediscoveryagent.h"
#include "qbluetoothdevicediscoveryagent_p.h"
#include "qbluetoothaddress.h"

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"

QTM_BEGIN_NAMESPACE

QBluetoothDeviceDiscoveryAgentPrivate::QBluetoothDeviceDiscoveryAgentPrivate()
:   adapter(0)
{
    manager = new OrgBluezManagerInterface(QLatin1String("org.bluez"), QLatin1String("/"),
                                           QDBusConnection::systemBus());
}

QBluetoothDeviceDiscoveryAgentPrivate::~QBluetoothDeviceDiscoveryAgentPrivate()
{
    delete manager;
}

bool QBluetoothDeviceDiscoveryAgent::isActive() const
{
    Q_D(const QBluetoothDeviceDiscoveryAgent);

    return d->adapter != 0;
}

void QBluetoothDeviceDiscoveryAgentPrivate::start()
{
    Q_Q(QBluetoothDeviceDiscoveryAgent);

    QDBusPendingReply<QDBusObjectPath> reply = manager->DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError())
        return;

    adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"), reply.value().path(),
                                           QDBusConnection::systemBus());

    QObject::connect(adapter, SIGNAL(DeviceFound(QString,QVariantMap)),
                     q, SLOT(_q_deviceFound(QString,QVariantMap)));
    QObject::connect(adapter, SIGNAL(PropertyChanged(QString,QDBusVariant)),
                     q, SLOT(_q_propertyChanged(QString,QDBusVariant)));

    QDBusPendingReply<> discoveryReply = adapter->StartDiscovery();
    if (discoveryReply.isError()) {
        delete adapter;
        adapter = 0;
        return;
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::stop()
{
    if (adapter) {
        adapter->StopDiscovery();
        adapter->deleteLater();
        adapter = 0;
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::_q_deviceFound(const QString &address,
                                                           const QVariantMap &dict)
{
    Q_Q(QBluetoothDeviceDiscoveryAgent);

    const QBluetoothAddress btAddress(address);
    const QString btName = dict.value(QLatin1String("Name")).toString();
    quint32 btClass = dict.value(QLatin1String("Class")).toUInt();

    QBluetoothDeviceInfo device(btAddress, btName, btClass);
    discoveredDevices.append(device);
    emit q->deviceDiscovered(device);
}

void QBluetoothDeviceDiscoveryAgentPrivate::_q_propertyChanged(const QString &name,
                                                               const QDBusVariant &value)
{
    Q_Q(QBluetoothDeviceDiscoveryAgent);

    if (name == QLatin1String("Discovering") && !value.variant().toBool()) {
        adapter->deleteLater();
        adapter = 0;
        emit q->finished();
    }
}

QTM_END_NAMESPACE
