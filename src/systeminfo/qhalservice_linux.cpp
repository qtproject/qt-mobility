/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QObject>
#include <QList>
#include <QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusObjectPath>

#include "qhalservice_linux_p.h"

QTM_BEGIN_NAMESPACE

static QDBusConnection dbusConnection = QDBusConnection::systemBus();

class QHalInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
    bool valid;
};

QHalInterface::QHalInterface(QObject *parent)
        : QObject(parent)
{
    d = new QHalInterfacePrivate();
    d->connectionInterface = new QDBusInterface(QLatin1String(HAL_DBUS_SERVICE),
                                                QLatin1String(HAL_DBUS_MANAGER_PATH),
                                                QLatin1String(HAL_DBUS_MANAGER_INTERFACE),
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        qDebug() << "Could not find Hal";
        d->valid = false;
        return;
    } else {
        d->valid = true;
    }
}

QHalInterface::~QHalInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QHalInterface::isValid()
{
    return d->valid;
}

QDBusInterface *QHalInterface::connectionInterface()  const
{
    return d->connectionInterface;
}

QStringList QHalInterface::findDeviceByCapability(const QString &cap)
{
    QDBusReply<  QStringList > reply = d->connectionInterface->call(QLatin1String("FindDeviceByCapability"), cap);
    if (!reply.isValid()) {
//        qDebug() << reply.error().message();
    } else {

        return reply.value();
    }
    return QStringList();
}

QStringList QHalInterface::getAllDevices()
{
    QDBusReply<  QStringList > reply = d->connectionInterface->call(QLatin1String("GetAllDevices"));
    if (!reply.isValid()) {
//        qDebug() << reply.error().message();
    } else {

        return reply.value();
    }
    return QStringList();
}

bool QHalInterface::deviceExists(const QString &path)
{
    QDBusReply< bool > reply = d->connectionInterface->call(QLatin1String("DeviceExists"), path);
    if (!reply.isValid()) {
        return reply.value();
    }
    return false;
}
/////////

class QHalDeviceInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QHalDeviceInterface::QHalDeviceInterface(const QString &devicePathName, QObject *parent )
        : QObject(parent)
{/*
 qDBusRegisterMetaType<QHalPropertyList>();*/

    d = new QHalDeviceInterfacePrivate();
    d->path = devicePathName;
    d->connectionInterface = new QDBusInterface(QLatin1String(HAL_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(HAL_DEVICE_INTERFACE),
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        qDebug() << "Could not find HalDeviceInterface";
        return;
    } else {
        d->valid = true;
    }
}

QHalDeviceInterface::~QHalDeviceInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QHalDeviceInterface::isValid()
{
    return d->valid;
}

bool QHalDeviceInterface::setConnections()
{
    if(!isValid() )
        return false;
    bool allOk = false;

    if (dbusConnection.connect(QLatin1String(HAL_DBUS_SERVICE),
                               d->path,
                               QLatin1String(HAL_DEVICE_INTERFACE),
                               QLatin1String("PropertyModified"),
                               this,SIGNAL(propertyModified( int, QVariantList)))) {
        allOk = true;
    }
    return allOk;
}

bool QHalDeviceInterface::getPropertyBool(const QString &prop)
{
    QDBusReply< bool > reply = d->connectionInterface->call(QLatin1String("GetPropertyBoolean"), prop);
    if ( reply.isValid() ) {
        return reply.value();
    } else {
//        qDebug() << reply.error().message();
    }
    return false;
}

QString QHalDeviceInterface::getPropertyString(const QString &prop)
{
    QDBusReply< QString > reply = d->connectionInterface->call(QLatin1String("GetPropertyString"), prop);
    if ( reply.isValid() ) {
        return reply.value();
    } else {
//        qDebug() << reply.error().message();
    }
    return QString();
}

QStringList QHalDeviceInterface::getPropertyStringList(const QString &prop)
{
    QDBusReply< QStringList > reply = d->connectionInterface->call(QLatin1String("GetPropertyStringList"), prop);
    if ( reply.isValid() ) {
        return reply.value();
    } else {
//        qDebug() << reply.error().message();
    }
    return QStringList();
}

qint32 QHalDeviceInterface::getPropertyInt(const QString &prop)
{
    QDBusReply< qint32 > reply = d->connectionInterface->call(QLatin1String("GetPropertyInteger"), prop);
    if ( reply.isValid() ) {
        return reply.value();
    } else {
//        qDebug() << reply.error().message();
    }
    return 0;
}


bool QHalDeviceInterface::queryCapability(const QString &cap)
{
    QDBusReply< bool > reply = d->connectionInterface->call(QLatin1String("QueryCapability"), cap);
    if ( reply.isValid() ) {
        return reply.value();
    } else {
//        qDebug() << reply.error().message();
    }
    return false;
}



bool QHalDeviceInterface::propertyExists(const QString &prop)
{
    QDBusReply< bool > reply = d->connectionInterface->call(QLatin1String("PropertyExists"), prop);
    if ( reply.isValid() ) {
        return reply.value();
    } else {
//        qDebug() << reply.error().message();
    }
    return false;

}

/////////

class QHalDeviceLaptopPanelInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QHalDeviceLaptopPanelInterface::QHalDeviceLaptopPanelInterface(const QString &devicePathName, QObject *parent )
        : QObject(parent)
{
    d = new QHalDeviceLaptopPanelInterfacePrivate();
    d->path = devicePathName;
    d->connectionInterface = new QDBusInterface(QLatin1String(HAL_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(HAL_DEVICES_LAPTOPPANEL_INTERFACE),
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        qDebug() << "Could not find HalDeviceLaptopPanelInterface";
        return;
    } else {
        d->valid = true;
    }
}

QHalDeviceLaptopPanelInterface::~QHalDeviceLaptopPanelInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QHalDeviceLaptopPanelInterface::isValid()
{
    return d->valid;
}

quint32 QHalDeviceLaptopPanelInterface::getBrightness()
{
    QDBusReply< qint32 > reply = d->connectionInterface->call(QLatin1String("GetBrightness"));
    if ( reply.isValid() ) {
//        qDebug() << __FUNCTION__ << reply.value();
        return reply.value();
    }
    return -1;
}

void QHalDeviceLaptopPanelInterface::setBrightness(quint32 brightness)
{
    QDBusReply< qint32 > reply = d->connectionInterface->call(QLatin1String("SetBrightness"), brightness);
    if ( reply.isValid() ) {
//        qDebug() << __FUNCTION__ << reply.value();
    }
}

////////////////
class QHalDeviceKillSwitchInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QHalDeviceKillSwitchInterface::QHalDeviceKillSwitchInterface(const QString &devicePathName, QObject *parent )
        : QObject(parent)
{
    d = new QHalDeviceKillSwitchInterfacePrivate();
    d->path = devicePathName;
    d->connectionInterface = new QDBusInterface(QLatin1String(HAL_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(HAL_DEVICE_KILLSWITCH_INTERFACE),
                                                dbusConnection);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        qDebug() << "Could not find HalDeviceLaptopPanelInterface";
        return;
    } else {
        d->valid = true;
    }
}

QHalDeviceKillSwitchInterface::~QHalDeviceKillSwitchInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QHalDeviceKillSwitchInterface::isValid()
{
    return d->valid;
}

quint32 QHalDeviceKillSwitchInterface::getPower()
{
    QDBusReply< qint32 > reply = d->connectionInterface->call(QLatin1String("GetPower"));
    if ( reply.isValid() ) {
        qDebug() << __FUNCTION__ << reply.value();
        return reply.value();
    }
    return -1;
}

#include "moc_qhalservice_linux_p.cpp"

QTM_END_NAMESPACE
