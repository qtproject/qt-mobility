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

#include "qbluetoothservicediscoveryagent.h"
#include "qbluetoothservicediscoveryagent_p.h"

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"
#include "bluez/device_p.h"

#include <QtDBus/QDBusPendingCallWatcher>

#define QTM_SERVICEDISCOVERY_DEBUG

#ifdef QTM_SERVICEDISCOVERY_DEBUG
#include <QtCore/QDebug>
#endif

QTM_BEGIN_NAMESPACE

QBluetoothServiceDiscoveryAgentPrivate::QBluetoothServiceDiscoveryAgentPrivate(const QBluetoothAddress &address)
:   error(QBluetoothServiceDiscoveryAgent::NoError), state(Inactive), deviceAddress(address),
    deviceDiscoveryAgent(0), mode(QBluetoothServiceDiscoveryAgent::MinimalDiscovery), manager(0), device(0)
{
    qRegisterMetaType<ServiceMap>("ServiceMap");
    qDBusRegisterMetaType<ServiceMap>();
}

QBluetoothServiceDiscoveryAgentPrivate::~QBluetoothServiceDiscoveryAgentPrivate()
{
    delete device;
    delete manager;
}

void QBluetoothServiceDiscoveryAgentPrivate::start(const QBluetoothAddress &address)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);    

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << "Full discovery on: " << address.toString();
#endif

    manager = new OrgBluezManagerInterface(QLatin1String("org.bluez"), QLatin1String("/"),
                                           QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> reply = manager->DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError()) {
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
        _q_serviceDiscoveryFinished();
        return;
    }

    adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"), reply.value().path(),
                                           QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> deviceObjectPath = adapter->CreateDevice(address.toString());

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(deviceObjectPath, q);
    watcher->setProperty("_q_BTaddress", QVariant::fromValue(address));
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     q, SLOT(_q_createdDevice(QDBusPendingCallWatcher*)));

}

bool QBluetoothServiceDiscoveryAgentPrivate::quickDiscovery(const QBluetoothAddress &address, const QBluetoothDeviceInfo &info)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    manager = new OrgBluezManagerInterface(QLatin1String("org.bluez"), QLatin1String("/"),
                                           QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> reply = manager->DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError()) {
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
        _q_serviceDiscoveryFinished();
        return false;
    }

    adapter = new OrgBluezAdapterInterface(QLatin1String("org.bluez"), reply.value().path(),
                                           QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> deviceObjectPath = adapter->FindDevice(address.toString());
    deviceObjectPath.waitForFinished();
    if (deviceObjectPath.isError()) {
#ifdef QTM_SERVICEDISCOVERY_DEBUG
        qDebug() << "Can't find:" << address.toString() << "Error: " << deviceObjectPath.error().message();
#endif
        return false;
    }
    device = new OrgBluezDeviceInterface(QLatin1String("org.bluez"),
                                         deviceObjectPath.value().path(),
                                         QDBusConnection::systemBus());

    QDBusPendingReply<QVariantMap> deviceReply = device->GetProperties();
    deviceReply.waitForFinished();
    if(deviceReply.isError())
        return false;
    QVariantMap v = deviceReply.value();
    QStringList device_uuids = v.value("UUIDs").toStringList();    

    if(device_uuids.empty() && !uuidFilter.isEmpty()){
        return false;
    }

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << "Quick discovery on" << address.toString();
#endif

    bool foundDevice = true;

    if(!uuidFilter.isEmpty()) {
        foundDevice = false;
        foreach (const QBluetoothUuid &uuid, uuidFilter) {
            foreach (const QString s, device_uuids){
                if(QBluetoothUuid(s) == uuid){
                    foundDevice = true;
#ifdef QTM_SERVICEDISCOVERY_DEBUG
                    qDebug() << "*** FOUND SERVICE ON " << address.toString() << "***";
#endif
                    goto done;
                }
            }
        }
    }

done:
    if(foundDevice) {
        QBluetoothServiceInfo serviceInfo;

        serviceInfo.setDevice(info);
        serviceInfo.setAttribute(QBluetoothServiceInfo::ProtocolDescriptorList,
                                 QBluetoothServiceInfo::Sequence(v.values("UUIDs")));

        Q_Q(QBluetoothServiceDiscoveryAgent);

        discoveredServices.append(serviceInfo);
        emit q->serviceDiscovered(serviceInfo);
#ifdef QTM_SERVICEDISCOVERY_DEBUG
        qDebug() << "Quick: " << serviceInfo.device().address().toString() << serviceInfo.serviceName();
#endif
        return true;
    }    

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << "Device did not match UUID fitler" << address.toString() << device_uuids.count();
#endif
    return true; // no uuid
}


void QBluetoothServiceDiscoveryAgentPrivate::stop()
{
#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << Q_FUNC_INFO << "Stop called";
#endif
    if(device){
        QDBusPendingReply<> reply = device->CancelDiscovery();
        reply.waitForFinished();

        discoveredDevices.clear();
        // with no more device this will stop discovery
        startServiceDiscovery();
//        qDebug() << "Stop done";
    }
}

void QBluetoothServiceDiscoveryAgentPrivate::_q_createdDevice(QDBusPendingCallWatcher *watcher)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

    const QBluetoothAddress &address = watcher->property("_q_BTaddress").value<QBluetoothAddress>();

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << Q_FUNC_INFO << "created" << address.toString();
#endif

    QDBusPendingReply<QDBusObjectPath> deviceObjectPath = *watcher;
    if (deviceObjectPath.isError()) {
        if (deviceObjectPath.error().name() != QLatin1String("org.bluez.Error.AlreadyExists")) {
            // Can't create device, maybe not present, not a fault to report
            // since this is normal, devices aren't always present
//            error = QBluetoothServiceDiscoveryAgent::UnknownError;
//            emit q->error(error);
            _q_serviceDiscoveryFinished();
#ifdef QTM_SERVICEDISCOVERY_DEBUG
            qDebug() << "Create device failed Error: " << error << deviceObjectPath.error().name();
#endif
            return;
        }

        deviceObjectPath = adapter->FindDevice(address.toString());
        deviceObjectPath.waitForFinished();
        if (deviceObjectPath.isError()) {
            error = QBluetoothServiceDiscoveryAgent::DeviceDiscoveryError;
            emit q->error(error);
            _q_serviceDiscoveryFinished();
#ifdef QTM_SERVICEDISCOVERY_DEBUG
            qDebug() << "Can't find device after creation Error: " << error << deviceObjectPath.error().name();
#endif
            return;
        }
    }

    device = new OrgBluezDeviceInterface(QLatin1String("org.bluez"),
                                         deviceObjectPath.value().path(),
                                         QDBusConnection::systemBus());

    QDBusPendingReply<QVariantMap> deviceReply = device->GetProperties();
    deviceReply.waitForFinished();
    if(deviceReply.isError())
        return;
    QVariantMap v = deviceReply.value();
    QStringList device_uuids = v.value("UUIDs").toStringList();

    QString pattern;
    foreach (const QBluetoothUuid &uuid, uuidFilter)
        pattern += uuid.toString().remove(QChar('{')).remove(QChar('}')) + QLatin1Char(' ');

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << Q_FUNC_INFO << "Discover: " << pattern.trimmed();
#endif
    QDBusPendingReply<ServiceMap> discoverReply = device->DiscoverServices(pattern.trimmed());
    watcher = new QDBusPendingCallWatcher(discoverReply, q);
    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     q, SLOT(_q_discoveredServices(QDBusPendingCallWatcher*)));

}

void QBluetoothServiceDiscoveryAgentPrivate::_q_discoveredServices(QDBusPendingCallWatcher *watcher)
{
    Q_Q(QBluetoothServiceDiscoveryAgent);

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << Q_FUNC_INFO;
#endif

    QDBusPendingReply<ServiceMap> reply = *watcher;
    if (reply.isError()) {
#ifdef QTM_SERVICEDISCOVERY_DEBUG
        qDebug() << "discoveredServices error: " << error << reply.error().message();
#endif
        watcher->deleteLater();
        error = QBluetoothServiceDiscoveryAgent::UnknownError;
        emit q->error(error);
        _q_serviceDiscoveryFinished();        
        return;
    }

    ServiceMap map = reply.value();

#ifdef QTM_SERVICEDISCOVERY_DEBUG
    qDebug() << "Parsing xml" << discoveredDevices.at(0).address().toString() << discoveredDevices.count() << map.count();
#endif

    foreach (const QString &record, reply.value()) {
        QXmlStreamReader xml(record);

#ifdef QTM_SERVICEDISCOVERY_DEBUG
      //  qDebug() << "Service xml" << record;
#endif

        QBluetoothServiceInfo serviceInfo;
        serviceInfo.setDevice(discoveredDevices.at(0));

        while (!xml.atEnd()) {
            xml.readNext();

            if (xml.tokenType() == QXmlStreamReader::StartElement &&
                xml.name() == QLatin1String("attribute")) {
                quint16 attributeId =
                    xml.attributes().value(QLatin1String("id")).toString().toUShort(0, 0);

                if (xml.readNextStartElement()) {
                    QVariant value = readAttributeValue(xml);

                    serviceInfo.setAttribute(attributeId, value);
                }
            }
        }

        if (!serviceInfo.isValid())
            continue;

        Q_Q(QBluetoothServiceDiscoveryAgent);

        discoveredServices.append(serviceInfo);
#ifdef QTM_SERVICEDISCOVERY_DEBUG
        qDebug() << "Discovered services" << discoveredDevices.at(0).address().toString();
#endif
        emit q->serviceDiscovered(serviceInfo);
        // could stop discovery, check for state
        if(discoveryState() == Inactive){
            qDebug() << "Exit discovery after stop";
            break;
        }
    }

    watcher->deleteLater();

    _q_serviceDiscoveryFinished();
}

QVariant QBluetoothServiceDiscoveryAgentPrivate::readAttributeValue(QXmlStreamReader &xml)
{
    if (xml.name() == QLatin1String("boolean")) {
        const QString value = xml.attributes().value(QLatin1String("value")).toString();
        xml.skipCurrentElement();
        return value == QLatin1String("true");
    } else if (xml.name() == QLatin1String("uint8")) {
        quint8 value = xml.attributes().value(QLatin1String("value")).toString().toUShort(0, 0);
        xml.skipCurrentElement();
        return value;
    } else if (xml.name() == QLatin1String("uint16")) {
        quint16 value = xml.attributes().value(QLatin1String("value")).toString().toUShort(0, 0);
        xml.skipCurrentElement();
        return value;
    } else if (xml.name() == QLatin1String("uint32")) {
        quint32 value = xml.attributes().value(QLatin1String("value")).toString().toUInt(0, 0);
        xml.skipCurrentElement();
        return value;
    } else if (xml.name() == QLatin1String("uint64")) {
        quint64 value = xml.attributes().value(QLatin1String("value")).toString().toULongLong(0, 0);
        xml.skipCurrentElement();
        return value;
    } else if (xml.name() == QLatin1String("uuid")) {
        QBluetoothUuid uuid;
        const QString value = xml.attributes().value(QLatin1String("value")).toString();
        if (value.startsWith(QLatin1String("0x"))) {
            if (value.length() == 6) {
                quint16 v = value.toUShort(0, 0);
                uuid = QBluetoothUuid(v);
            } else if (value.length() == 10) {
                quint32 v = value.toUInt(0, 0);
                uuid = QBluetoothUuid(v);
            }
        } else {
            uuid = QBluetoothUuid(value);
        }
        xml.skipCurrentElement();
        return QVariant::fromValue(uuid);
    } else if (xml.name() == QLatin1String("text")) {
        QString value = xml.attributes().value(QLatin1String("value")).toString();
        if (xml.attributes().value(QLatin1String("encoding")) == QLatin1String("hex"))
            value = QString::fromUtf8(QByteArray::fromHex(value.toAscii()));
        xml.skipCurrentElement();
        return value;
    } else if (xml.name() == QLatin1String("sequence")) {
        QBluetoothServiceInfo::Sequence sequence;

        while (xml.readNextStartElement()) {
            QVariant value = readAttributeValue(xml);
            sequence.append(value);
        }

        return QVariant::fromValue<QBluetoothServiceInfo::Sequence>(sequence);
    } else {
        qWarning("unknown attribute type %s %s",
                 xml.name().toString().toLocal8Bit().constData(),
                 xml.attributes().value(QLatin1String("value")).toString().toLocal8Bit().constData());
        Q_ASSERT(false);
        xml.skipCurrentElement();
        return QVariant();
    }
}

QTM_END_NAMESPACE
