/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qconnmanservice_linux_p.h"
#include <QtDBus/qdbusreply.h>

#ifndef QT_NO_DBUS

QTM_BEGIN_NAMESPACE

#define CONNMAN_SERVICE              "net.connman"
#define CONNMAN_MANAGER_INTERFACE    CONNMAN_SERVICE ".Manager"
#define CONNMAN_MANAGER_PATH         "/"
#define CONNMAN_SERVICE_INTERFACE    CONNMAN_SERVICE ".Service"
#define CONNMAN_TECHNOLOGY_INTERFACE CONNMAN_SERVICE ".Technology"

QConnmanManagerInterface::QConnmanManagerInterface(QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE), QLatin1String(CONNMAN_MANAGER_PATH),
                             CONNMAN_MANAGER_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QConnmanManagerInterface::~QConnmanManagerInterface()
{
}

void QConnmanManagerInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QConnmanDBusHelper *helper = new QConnmanDBusHelper(this);
        QDBusConnection::systemBus().connect(QLatin1String(CONNMAN_SERVICE),
                                             QLatin1String(CONNMAN_MANAGER_PATH),
                                             QLatin1String(CONNMAN_MANAGER_INTERFACE),
                                             QLatin1String("PropertyChanged"),
                                             helper, SLOT(propertyChanged(QString,QDBusVariant)));
        connect(helper, SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this, SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                Qt::UniqueConnection);
    }
}

void QConnmanManagerInterface::disconnectNotify(const char */*signal*/)
{
    // TODO: disconnect the signal
}

QVariant QConnmanManagerInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QDBusObjectPath QConnmanManagerInterface::lookupService(const QString &service)
{
//    QDBusReply<QDBusObjectPath > reply = this->call(QLatin1String("LookupService"), QVariant::fromValue(service));
//    if(!reply.isValid() || reply.error().message().contains("Not found")) {
// LookupService is broke in connman
    foreach (const QString servicepath, getServices()) {
        if (!servicepath.isEmpty()) {
            QConnmanServiceInterface serv(servicepath, this);
            if (serv.getName() == service || serv.getType() == service)
                return QDBusObjectPath(servicepath);

            QString techpath = getPathForTechnology(serv.getType());
            QConnmanTechnologyInterface tech(techpath);
            if (tech.isValid()) {
                if (serv.getInterface() == service || serv.getName() == service)
                    return QDBusObjectPath(servicepath);
            }
        }
    }
//     }
    return QDBusObjectPath();
}

QString QConnmanManagerInterface::getDefaultTechnology()
{
    QVariant var = getProperty("DefaultTechnology");
    return qdbus_cast<QString>(var);
}

QStringList QConnmanManagerInterface::getTechnologies()
{
    QVariant var = getProperty("Technologies");
    return qdbus_cast<QStringList >(var);
}

QStringList QConnmanManagerInterface::getServices()
{
    QVariant var = getProperty("Services");
    return qdbus_cast<QStringList >(var);
}

QString QConnmanManagerInterface::getPathForTechnology(const QString &name)
{
    foreach (const QString path, getTechnologies()) {
        if (path.contains(name))
            return path;
    }
    return QString();
}

QConnmanServiceInterface::QConnmanServiceInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE), dbusPathName,
                             CONNMAN_SERVICE_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QConnmanServiceInterface::~QConnmanServiceInterface()
{
}

void QConnmanServiceInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QConnmanDBusHelper *helper = new QConnmanDBusHelper(this);
        QDBusConnection::systemBus().connect(QLatin1String(CONNMAN_SERVICE),
                                             path(),
                                             QLatin1String(CONNMAN_SERVICE_INTERFACE),
                                             QLatin1String("PropertyChanged"),
                                             helper, SLOT(propertyChanged(QString,QDBusVariant)));
        connect(helper, SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this, SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                Qt::UniqueConnection);
    }
}

void QConnmanServiceInterface::disconnectNotify(const char */*signal*/)
{
    // TODO: disconnect the signal
}

QVariant QConnmanServiceInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QString QConnmanServiceInterface::getState()
{
    QVariant var = getProperty("State");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

QString QConnmanServiceInterface::getType()
{
    QVariant var = getProperty("Type");
    return qdbus_cast<QString>(var);
}

quint8 QConnmanServiceInterface::getSignalStrength()
{
    QVariant var = getProperty("Strength");
    return qdbus_cast<quint8>(var);
}

QString QConnmanServiceInterface::getAPN()
{
    QVariant var = getProperty("APN");
    return qdbus_cast<QString>(var);
}

bool QConnmanServiceInterface::isRoaming()
{
    QVariant var = getProperty("Roaming");
    return qdbus_cast<bool>(var);
}

QStringList QConnmanServiceInterface::getDomains()
{
    QVariant var = getProperty("Domains");
    return qdbus_cast<QStringList>(var);
}

QVariantMap QConnmanServiceInterface::getEthernet()
{
    QVariant var = getProperty("Ethernet");
    return qdbus_cast<QVariantMap>(var);
}

QString QConnmanServiceInterface::getInterface()
{
    QVariantMap map = getEthernet();
    QMapIterator<QString, QVariant> it(map);
    while (it.hasNext()) {
        it.next();
        if (it.key() == "Interface")
            return it.value().toString();
    }
    return QString();
}

QString QConnmanServiceInterface::getMacAddress()
{
    QVariantMap map = getEthernet();
    QMapIterator<QString, QVariant> it(map);
    while (it.hasNext()) {
        it.next();
        if (it.key() == "Address")
            return it.value().toString();
    }
    return QString();
}

QConnmanTechnologyInterface::QConnmanTechnologyInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(CONNMAN_SERVICE), dbusPathName,
                             CONNMAN_TECHNOLOGY_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QConnmanTechnologyInterface::~QConnmanTechnologyInterface()
{
}

QConnmanDBusHelper::QConnmanDBusHelper(QObject * parent)
    : QObject(parent)
{
}

QConnmanDBusHelper::~QConnmanDBusHelper()
{
}

void QConnmanDBusHelper::propertyChanged(const QString &item, const QDBusVariant &var)
{
    QDBusMessage msg = message();
    Q_EMIT propertyChangedContext(msg.path(), item, var);
}

#include "moc_qconnmanservice_linux_p.cpp"

QTM_END_NAMESPACE

#endif // QT_NO_DBUS
