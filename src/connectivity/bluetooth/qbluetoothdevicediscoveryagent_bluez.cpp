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
#include "qbluetoothuuid.h"

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"
#include "bluez/device_p.h"

#define QTM_DEVICEDISCOVERY_DEBUG

QTM_BEGIN_NAMESPACE

QBluetoothDeviceDiscoveryAgentPrivate::QBluetoothDeviceDiscoveryAgentPrivate()
    :   adapter(0), lastError(QBluetoothDeviceDiscoveryAgent::NoError)
{
    manager = new OrgBluezManagerInterface(QLatin1String("org.bluez"), QLatin1String("/"),
                                           QDBusConnection::systemBus());
    inquiryType = QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry;
}

QBluetoothDeviceDiscoveryAgentPrivate::~QBluetoothDeviceDiscoveryAgentPrivate()
{
    delete manager;
}

bool QBluetoothDeviceDiscoveryAgentPrivate::isActive() const
{
    return adapter != 0;
}

void QBluetoothDeviceDiscoveryAgentPrivate::start()
{
    discoveredDevices.clear();

    QDBusPendingReply<QDBusObjectPath> reply = manager->DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError()) {
        errorString = reply.error().message();
#ifdef QTM_DEVICEDISCOVERY_DEBUG
        qDebug() << Q_FUNC_INFO << "ERROR: " << errorString;
#endif
        lastError = QBluetoothDeviceDiscoveryAgent::IOFailure;
        Q_Q(QBluetoothDeviceDiscoveryAgent);
        emit q->error(lastError);
        return;
    }

    adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"), reply.value().path(),
                                           QDBusConnection::systemBus());

    Q_Q(QBluetoothDeviceDiscoveryAgent);
    QObject::connect(adapter, SIGNAL(DeviceFound(QString,QVariantMap)),
                     q, SLOT(_q_deviceFound(QString,QVariantMap)));
    QObject::connect(adapter, SIGNAL(PropertyChanged(QString,QDBusVariant)),
                     q, SLOT(_q_propertyChanged(QString,QDBusVariant)));

    QDBusPendingReply<QVariantMap> propertiesReply = adapter->GetProperties();
    propertiesReply.waitForFinished();
    if(propertiesReply.isError()) {
#ifdef QTM_DEVICEDISCOVERY_DEBUG
        qDebug() << Q_FUNC_INFO << "ERROR: " << errorString;
#endif
        errorString = propertiesReply.error().message();
        lastError = QBluetoothDeviceDiscoveryAgent::IOFailure;
        Q_Q(QBluetoothDeviceDiscoveryAgent);
        emit q->error(lastError);
        return;
    }

#ifdef QTM_DEVICEDISCOVERY_DEBUG
    qDebug() << "Looking up cached devices";
#endif

    QVariant p = propertiesReply.value()["Devices"];
    QDBusArgument d = p.value<QDBusArgument>();
    QStringList l; d >> l;
    QString path;
    foreach (path, l){        
        OrgBluezDeviceInterface *device = new OrgBluezDeviceInterface(QLatin1String("org.bluez"), path,
                                                                      QDBusConnection::systemBus());
        QDBusPendingReply<QVariantMap> deviceReply = device->GetProperties();
        deviceReply.waitForFinished();
        if(deviceReply.isError())
            continue;
        QVariantMap v = deviceReply.value();
        QString address = v.value("Address").toString();
        v.insert(QLatin1String("Cached"), QVariant(true));
#ifdef QTM_DEVICEDISCOVERY_DEBUG
        qDebug() << "Cached Address: " << address << "Num UUIDs:" << v.value("UUIDs").toStringList().count();
#endif
        _q_deviceFound(address, v);
    }

#ifdef QTM_DEVICEDISCOVERY_DEBUG
    qDebug() << "Starting discovery...";
#endif

    QDBusPendingReply<> discoveryReply = adapter->StartDiscovery();
    if (discoveryReply.isError()) {
        delete adapter;
        adapter = 0;
        errorString = discoveryReply.error().message();
        lastError = QBluetoothDeviceDiscoveryAgent::IOFailure;
        Q_Q(QBluetoothDeviceDiscoveryAgent);
        emit q->error(lastError);
#ifdef QTM_DEVICEDISCOVERY_DEBUG
        qDebug() << Q_FUNC_INFO << "ERROR: " << errorString;
#endif
        return;
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::stop()
{
    if (adapter) {
#ifdef QTM_DEVICEDISCOVERY_DEBUG
        qDebug() << Q_FUNC_INFO;
#endif
        adapter->StopDiscovery();
        adapter->deleteLater();
        adapter = 0;
        Q_Q(QBluetoothDeviceDiscoveryAgent);
        emit q->error(QBluetoothDeviceDiscoveryAgent::Canceled);
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::_q_deviceFound(const QString &address,
                                                           const QVariantMap &dict)
{
    const QBluetoothAddress btAddress(address);
    const QString btName = dict.value(QLatin1String("Name")).toString();
    quint32 btClass = dict.value(QLatin1String("Class")).toUInt();    

#ifdef QTM_DEVICEDISCOVERY_DEBUG
    qDebug() << "Discovered: " << address << btName << "Num UUIDs" << dict.value(QLatin1String("UUIDs")).toStringList().count() << "total device" << discoveredDevices.count() << "cached" << dict.value(QLatin1String("Cached")).toBool();
#endif

    QBluetoothDeviceInfo device(btAddress, btName, btClass);    
    QList<QBluetoothUuid> uuids;
    foreach (QString u, dict.value(QLatin1String("UUIDs")).toStringList()) {
        uuids.append(QBluetoothUuid(u));
    }
    device.setServiceUuids(uuids, QBluetoothDeviceInfo::DataIncomplete);
    device.setCached(dict.value("Cached").toBool());
    for(int i = 0; i < discoveredDevices.size(); i++){
        if(discoveredDevices[i].address() == device.address()) {
            if(discoveredDevices[i] == device) {
#ifdef QTM_DEVICEDISCOVERY_DEBUG
                  qDebug() << "Duplicate: " << address;
#endif
                return;
            }
            discoveredDevices.replace(i, device);
            Q_Q(QBluetoothDeviceDiscoveryAgent);
#ifdef QTM_DEVICEDISCOVERY_DEBUG
            qDebug() << "Updated: " << address;
#endif

            emit q->deviceDiscovered(device);
            return; // this works if the list doesn't contain duplicates. Don't let it.
        }
    }
#ifdef QTM_DEVICEDISCOVERY_DEBUG
    qDebug() << "Emit: " << address;
#endif
    discoveredDevices.append(device);
    Q_Q(QBluetoothDeviceDiscoveryAgent);
    emit q->deviceDiscovered(device);
}

void QBluetoothDeviceDiscoveryAgentPrivate::_q_propertyChanged(const QString &name,
                                                               const QDBusVariant &value)
{    
#ifdef QTM_DEVICEDISCOVERY_DEBUG
    qDebug() << Q_FUNC_INFO << name << value.variant();
#endif
    if (name == QLatin1String("Discovering") && !value.variant().toBool()) {
        Q_Q(QBluetoothDeviceDiscoveryAgent);
        adapter->deleteLater();
        adapter = 0;
        emit q->finished();
    }
}

QTM_END_NAMESPACE
