/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdevicekitservice_linux_p.h"

QTM_BEGIN_NAMESPACE

#if !defined(QT_NO_UDISKS)

#define	UDISKS_SERVICE        "org.freedesktop.UDisks"
#define	UDISKS_PATH           "/org/freedesktop/UDisks"
#define	UDISKS_DEVICE_SERVICE "org.freedesktop.UDisks.Device"
#define	UDISKS_DEVICE_PATH    "/org/freedesktop/UDisks/Device"

QUDisksInterface::QUDisksInterface(QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UDISKS_SERVICE)
    , QLatin1String(UDISKS_PATH)
    , UDISKS_SERVICE
    , QDBusConnection::systemBus()
    , parent)
{
}

QUDisksInterface::~QUDisksInterface()
{
}

QList<QDBusObjectPath> QUDisksInterface::enumerateDevices()
{
    QDBusReply<QList<QDBusObjectPath> > reply = call(QLatin1String("EnumerateDevices"));
    return reply.value();
}

void QUDisksInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(deviceChanged(QDBusObjectPath))) {
        if (!connection().connect(QLatin1String(UDISKS_SERVICE),
                                  UDISKS_PATH,
                                  UDISKS_SERVICE,
                                  "DeviceChanged",
                                  this, SIGNAL(deviceChanged(QDBusObjectPath)))) {
            qDebug() << "This is messed up"<<connection().lastError().message();
        }
    }
}

void QUDisksInterface::disconnectNotify(const char */*signal*/)
{
}

QUDisksDeviceInterface::QUDisksDeviceInterface(const QString &dbusPathName,QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UDISKS_SERVICE)
    , dbusPathName
    , UDISKS_DEVICE_SERVICE
    , QDBusConnection::systemBus()
    , parent)
{
    path = dbusPathName;
}

QUDisksDeviceInterface::~QUDisksDeviceInterface()
{
}

bool QUDisksDeviceInterface::deviceIsMounted()
{
    return getProperty("DeviceIsMounted").toBool();
}

bool QUDisksDeviceInterface::deviceIsRemovable()
{
    return getProperty("DeviceIsRemovable").toBool();
}

QStringList QUDisksDeviceInterface::deviceMountPaths()
{
    return getProperty("DeviceMountPaths").toStringList();
}

bool QUDisksDeviceInterface::deviceIsSystemInternal()
{
    return getProperty("DeviceIsSystemInternal").toBool();
}

QString QUDisksDeviceInterface::deviceFilePresentation()
{
    return getProperty("DeviceFilePresentation").toString();
}

bool QUDisksDeviceInterface::deviceIsLinuxLvm2LV()
{
    return getProperty("DeviceIsLinuxLvm2LV").toBool();
}

bool QUDisksDeviceInterface::deviceIsLinuxMd()
{
    return getProperty("DeviceIsLinuxMd").toBool();
}

bool QUDisksDeviceInterface::deviceIsLinuxLvm2PV()
{
    return getProperty("DeviceIsLinuxLvm2PV").toBool();
}

bool QUDisksDeviceInterface::driveIsRotational()
{
    return getProperty("DriveIsRotational").toBool();
}

QString QUDisksDeviceInterface::uuid()
{
//    if(deviceIsPartition()) {
//        return this->getProperty("PartitionUuid").toString();
//    }
    if (deviceIsLinuxLvm2LV())
        return getProperty("LinuxLvm2LVUuid").toString();

    if (deviceIsLinuxMd())
        return getProperty("LinuxMdUuid").toString();

    if (deviceIsLinuxLvm2PV())
        return getProperty("LinuxLvm2PVUuid").toString();

    return getProperty("IdUuid").toString();
}


QVariant QUDisksDeviceInterface::getProperty(const QString &property)
{
    QVariant var;
    QDBusInterface *iface = new QDBusInterface(UDISKS_SERVICE, path,
                                               "org.freedesktop.DBus.Properties",
                                               QDBusConnection::systemBus());
    if (iface && iface->isValid()) {
        QDBusReply<QVariant> r = iface->call("Get", path, property);
        var = r.value();
    }
    return var;
}

bool QUDisksDeviceInterface::deviceIsDrive()
{
    return this->getProperty("DeviceIsDrive").toBool();
}

QString QUDisksDeviceInterface::driveMedia()
{
    return this->getProperty("DriveMedia").toString();
}

bool QUDisksDeviceInterface::driveCanDetach()
{
    return this->getProperty("DriveCanDetach").toBool();
}

bool QUDisksDeviceInterface::driveIsMediaEjectable()
{
     return this->getProperty("DriveIsMediaEjectable").toBool();
}

#endif // QT_NO_UDISKS

#if !defined(QT_NO_UPOWER)

#define	UPOWER_SERVICE        "org.freedesktop.UPower"
#define	UPOWER_PATH           "/org/freedesktop/UPower"
#define	UPOWER_DEVICE_SERVICE "org.freedesktop.UPower.Device"
#define	UPOWER_DEVICE_PATH    "/org/freedesktop/UPower/Device"

QUPowerInterface::QUPowerInterface(QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UPOWER_SERVICE)
    , QLatin1String(UPOWER_PATH)
    , UPOWER_SERVICE
    , QDBusConnection::systemBus()
    , parent)
{
}

QUPowerInterface::~QUPowerInterface()
{
}

QList<QDBusObjectPath> QUPowerInterface::enumerateDevices()
{
    QDBusReply<QList<QDBusObjectPath> > reply = call(QLatin1String("EnumerateDevices"));
    return reply.value();
}

QVariant QUPowerInterface::getProperty(const QString &property)
{
    QVariant var;
    QDBusInterface *interface = new QDBusInterface(UPOWER_SERVICE, UPOWER_PATH,
                                             "org.freedesktop.DBus.Properties",
                                             QDBusConnection::systemBus());
    if (interface && interface->isValid()) {
        QDBusReply<QVariant> r = interface->call("Get", UPOWER_PATH, property);
        var = r.value();
    }
    return var;
}

void QUPowerInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(changed())) {
        if (!connection().connect(QLatin1String(UPOWER_SERVICE),
                                  UPOWER_PATH,
                                  UPOWER_SERVICE,
                                  QLatin1String("Changed"),
                                  this, SIGNAL(changed()))) {
            qDebug() << "Error"<<connection().lastError().message();
        }
    }
}
void QUPowerInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(changed(QString))) {
        if (!connection().disconnect(QLatin1String(UPOWER_SERVICE),
                                     UPOWER_PATH,
                                     UPOWER_SERVICE,
                                     QLatin1String("Changed"),
                                     this, SIGNAL(changed()))) {
            qDebug() << "Error"<<connection().lastError().message();
        }
    }
}

bool QUPowerInterface::onBattery()
{
    return getProperty("OnBattery").toBool();
}

QUPowerDeviceInterface::QUPowerDeviceInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UPOWER_SERVICE)
    , dbusPathName
    , UPOWER_DEVICE_SERVICE
    , QDBusConnection::systemBus()
    , parent)
{
    path = dbusPathName;
    propertiesInterface = new QDBusInterface(UPOWER_SERVICE, path,
                                             "org.freedesktop.DBus.Properties",
                                             QDBusConnection::systemBus());
    pMap = getProperties();
}

QUPowerDeviceInterface::~QUPowerDeviceInterface()
{
}

QVariantMap QUPowerDeviceInterface::getProperties()
{
    QDBusReply<QVariantMap> reply = propertiesInterface->call(QLatin1String("GetAll"), QLatin1String("org.freedesktop.UPower.Device"));
    if (!reply.isValid())
        qDebug() << reply.error();
    return reply.value();
}

QVariant QUPowerDeviceInterface::getProperty(const QString &property)
{
    QVariant var;
    if (propertiesInterface && propertiesInterface->isValid()) {
        QDBusReply<QVariant> r = propertiesInterface->call("Get", path, property);
        var = r.value();
    }
    return var;
}

quint16 QUPowerDeviceInterface::getType()
{
    return getProperty("Type").toUInt();
}

bool QUPowerDeviceInterface::isPowerSupply()
{
    return getProperty("PowerSupply").toBool();
}

bool QUPowerDeviceInterface::isOnline()
{
    return getProperty("Online").toBool();
}

double QUPowerDeviceInterface::currentEnergy()
{
    return getProperty("Energy").toDouble();
}

double QUPowerDeviceInterface::energyWhenFull()
{
    return getProperty("EnergyFull").toDouble();
}

double QUPowerDeviceInterface::energyDischargeRate()
{
    return getProperty("EnergyRate").toDouble();
}

double QUPowerDeviceInterface::voltage()
{
    return getProperty("Voltage").toDouble();
}

qint64 QUPowerDeviceInterface::timeToFull()
{
    return getProperty("TimeToFull").toUInt();
}

double QUPowerDeviceInterface::percentLeft()
{
    return getProperty("Percentage").toDouble();
}

quint16 QUPowerDeviceInterface::getState()
{
    return getProperty("State").toUInt();
}

void QUPowerDeviceInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(changed())) {
        if (!connection().connect(QLatin1String(UPOWER_SERVICE),
                                  path,
                                  UPOWER_DEVICE_SERVICE,
                                  QLatin1String("Changed"),
                                  this, SIGNAL(changed()))) {
            qDebug() << "Error" << connection().lastError().message();
        }
    } else if (QLatin1String(signal) == SIGNAL(propertyChanged(QString,QVariant))) {
        if (!connection().connect(QLatin1String(UPOWER_SERVICE),
                                  path,
                                  UPOWER_DEVICE_SERVICE,
                                  QLatin1String("Changed"),
                                  this, SIGNAL(propChanged()))) {
            qDebug() << "Error" << connection().lastError().message();
        }
    }
}

void QUPowerDeviceInterface::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(changed())) {
        if (!connection().disconnect(QLatin1String(UPOWER_SERVICE),
                                     path,
                                     UPOWER_DEVICE_SERVICE,
                                     QLatin1String("Changed"),
                                     this, SIGNAL(changed()))) {
            qDebug() << "Error" << connection().lastError().message();
        }
    }
}

void QUPowerDeviceInterface::propChanged()
{
    QVariantMap map = getProperties();
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        if (pMap.value(i.key()) != i.value()) {
            pMap[i.key()] = i.value();
            Q_EMIT propertyChanged(i.key(), QVariant::fromValue(i.value()));
        }
    }
}

#endif // QT_NO_UPOWER

#if !defined(QT_NO_UPOWER) || !defined(QT_NO_UDISKS)
#include "moc_qdevicekitservice_linux_p.cpp"
#endif

QTM_END_NAMESPACE
