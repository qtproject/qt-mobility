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

#include "qdevicekitservice_linux_p.h"

#include <QtDBus/QtDBus>
//#include <QtDBus/QDBusConnection>
//#include <QtDBus/QDBusError>
//#include <QtDBus/QDBusInterface>
//#include <QtDBus/QDBusMessage>
//#include <QtDBus/QDBusReply>
//#include <QtDBus/QDBusPendingCallWatcher>
//#include <QtDBus/QDBusObjectPath>
//#include <QtDBus/QDBusPendingCall>


QUDisksInterface::QUDisksInterface( QObject *parent)
      : QDBusAbstractInterface(QLatin1String(UDISKS_SERVICE),
                                 QLatin1String(UDISKS_PATH),
                                 UDISKS_SERVICE,
                                 QDBusConnection::systemBus(), parent)
{
    qDebug() << Q_FUNC_INFO;
}

QUDisksInterface::~QUDisksInterface()
{
}

//QDBusObjectPath QUDisksInterface::path() const
//{

//}

QList<QDBusObjectPath> QUDisksInterface::enumerateDevices()
{
    QDBusReply<QList<QDBusObjectPath> > reply =  this->call(QLatin1String("EnumerateDevices"));

    return reply.value();
}

QStringList QUDisksInterface::enumerateDeviceFiles()
{
    QDBusReply<QStringList > reply =  this->call(QLatin1String("EnumerateDevices"));
    return reply.value();
}

QDBusObjectPath QUDisksInterface::findDeviceByDeviceFile(const QString &path)
{
    QDBusReply<QDBusObjectPath > reply =  this->call(QLatin1String("FindDeviceByDeviceFile"));
    return reply.value();

}

QVariantMap QUDisksInterface::getProperties()
{
    QDBusReply<QVariantMap > reply = this->call(QLatin1String("GetAll"));
    return reply.value();
}

QVariant QUDisksInterface::getProperty(const QString &property)
{
    QVariant var;
    QVariantMap map = getProperties();
    if (map.contains(property)) {
        var = map.value(property);
    }
    return var;
}

void QUDisksInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(deviceAdded(QString))) {
        if(!connection().connect(QLatin1String(UDISKS_SERVICE),
                               UDISKS_PATH,
                               UDISKS_SERVICE,
                               QLatin1String("DeviceAdded"),
                               this,SIGNAL(deviceAdded(QDBusObjectPath)))) {
            qDebug() << "Error"<<connection().lastError().message();
        }
    }
    if (QLatin1String(signal) == SIGNAL(deviceChanged(QDBusObjectPath))) {
        if(!connection().connect(QLatin1String(UDISKS_SERVICE),
                               UDISKS_PATH,
                               UDISKS_SERVICE,
                               "DeviceChanged",
                               this,SIGNAL(deviceChanged(QDBusObjectPath)))) {
            qDebug() << "This is messed up"<<connection().lastError().message();
        }
    }
    if (QLatin1String(signal) == SIGNAL(deviceRemoved(QDBusObjectPath))) {
        connection().connect(QLatin1String(UDISKS_SERVICE),
                               UDISKS_PATH,
                               QLatin1String(UDISKS_SERVICE ),
                               QLatin1String("DeviceRemoved"),
                               this,SIGNAL(deviceRemoved(QDBusObjectPath)));
    }
}

void QUDisksInterface::disconnectNotify(const char *signal)
{
}

QUDisksDeviceInterface::QUDisksDeviceInterface(const QString &dbusPathName,QObject *parent)
    : QDBusAbstractInterface(QLatin1String(UDISKS_SERVICE),
                             dbusPathName,
                             UDISKS_DEVICE_SERVICE,
                             QDBusConnection::systemBus(), parent)
{
    path = dbusPathName;
}

QUDisksDeviceInterface::~QUDisksDeviceInterface()
{
}

bool QUDisksDeviceInterface::deviceIsMounted()
{
    return this->getProperty("DeviceIsMounted").toBool();
}

bool QUDisksDeviceInterface::deviceIsRemovable()
{
    return this->getProperty("DeviceIsRemovable").toBool();
}

bool QUDisksDeviceInterface::deviceIsMediaChangeDetacted()
{
    return this->getProperty("DeviceIsMediaChangeDetacted").toBool();
}

QStringList QUDisksDeviceInterface::deviceMountPaths()
{
    return this->getProperty("DeviceMountPaths").toStringList();
}

bool QUDisksDeviceInterface::deviceIsPartition()
{
    return this->getProperty("DeviceIsPartition").toBool();
}

qulonglong QUDisksDeviceInterface::partitionSize()
{
    return this->getProperty("PartitionSize").toULongLong();
}

bool QUDisksDeviceInterface::deviceIsSystemInternal()
{
    return this->getProperty("DeviceIsSystemInternal").toBool();
}

QString QUDisksDeviceInterface::deviceFilePresentation()
{
    return this->getProperty("DeviceFilePresentation").toString();
}

QString QUDisksDeviceInterface::deviceFile()
{
    return this->getProperty("DeviceFile").toString();
}

QString QUDisksDeviceInterface::driveMedia()
{
    return this->getProperty("DriveMedia").toString();
}

bool QUDisksDeviceInterface::deviceIsDrive()
{
    return this->getProperty("DeviceIsDrive").toBool();
}

QVariantMap QUDisksDeviceInterface::getProperties()
{
    QDBusReply<QVariantMap > reply = this->call(QLatin1String("GetAll"));
    return reply.value();
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
