/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qofonoservice_linux_p.h"
#include <QtDBus/qdbusargument.h>
#include <QtDBus/qdbusmetatype.h>
#include <QtDBus/qdbusreply.h>

#ifndef QT_NO_DBUS

QDBusArgument &operator<<(QDBusArgument &argument, const QOfonoProperties &property)
{
    argument.beginStructure();
    argument << property.path << property.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QOfonoProperties &property)
{
    argument.beginStructure();
    argument >> property.path >> property.properties;
    argument.endStructure();
    return argument;
}

QTM_BEGIN_NAMESPACE

#define OFONO_SERVICE                        "org.ofono"
#define OFONO_MANAGER_INTERFACE              "org.ofono.Manager"
#define OFONO_MANAGER_PATH                   "/"
#define OFONO_MODEM_INTERFACE                "org.ofono.Modem"
#define OFONO_NETWORK_REGISTRATION_INTERFACE "org.ofono.NetworkRegistration"
#define OFONO_NETWORK_OPERATOR_INTERFACE     "org.ofono.NetworkOperator"
#define OFONO_SIM_MANAGER_INTERFACE          "org.ofono.SimManager"
#define OFONO_CONNECTION_MANAGER_INTERFACE   "org.ofono.ConnectionManager"

QOfonoManagerInterface::QOfonoManagerInterface(QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE), QLatin1String(OFONO_MANAGER_PATH),
                             OFONO_MANAGER_INTERFACE, QDBusConnection::systemBus(), parent)
{
    qDBusRegisterMetaType<QOfonoProperties>();
    qDBusRegisterMetaType<QOfonoPropertyMap>();
}

QOfonoManagerInterface::~QOfonoManagerInterface()
{
}

QList<QDBusObjectPath> QOfonoManagerInterface::getModems()
{
    QDBusReply<QOfonoPropertyMap> reply = call(QLatin1String("GetModems"));
    QList<QDBusObjectPath> modems;
    if (reply.isValid()) {
        foreach (const QOfonoProperties &property, reply.value())
            modems << property.path;
    }
    return modems;
}

QDBusObjectPath QOfonoManagerInterface::currentModem()
{
    QList<QDBusObjectPath> modems = getModems();
    foreach (const QDBusObjectPath &modem, modems) {
        QOfonoModemInterface device(modem.path());
        if (device.isPowered())
            return modem;
    }
    return QDBusObjectPath();
}

void QOfonoManagerInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper = new QOfonoDBusHelper(this);
        QDBusConnection::systemBus().connect(QLatin1String(OFONO_SERVICE),
                                             QLatin1String(OFONO_MANAGER_PATH),
                                             QLatin1String(OFONO_MANAGER_INTERFACE),
                                             QLatin1String("PropertyChanged"),
                                             helper, SLOT(propertyChanged(QString,QDBusVariant)));
        connect(helper, SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)));
    }
}

void QOfonoManagerInterface::disconnectNotify(const char */*signal*/)
{
    // TODO: disconnect the DBus signal
}

QVariant QOfonoManagerInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QOfonoDBusHelper::QOfonoDBusHelper(QObject *parent)
    : QObject(parent)
{
}

QOfonoDBusHelper::~QOfonoDBusHelper()
{
}

void QOfonoDBusHelper::propertyChanged(const QString &item, const QDBusVariant &value)
{
    QDBusMessage msg = message();
    Q_EMIT propertyChangedContext(msg.path(), item, value);
}

QOfonoModemInterface::QOfonoModemInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE), dbusPathName,
                             OFONO_MODEM_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QOfonoModemInterface::~QOfonoModemInterface()
{
}

bool QOfonoModemInterface::isPowered()
{
    QVariant var = getProperty("Powered");
    return qdbus_cast<bool>(var);
}

QString QOfonoModemInterface::getSerial()
{
    QVariant var = getProperty("Serial");
    return qdbus_cast<QString>(var);
}

QVariant QOfonoModemInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QOfonoNetworkRegistrationInterface::QOfonoNetworkRegistrationInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE), dbusPathName,
                             OFONO_NETWORK_REGISTRATION_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QOfonoNetworkRegistrationInterface::~QOfonoNetworkRegistrationInterface()
{
}

QString QOfonoNetworkRegistrationInterface::getStatus()
{
    QVariant var = getProperty("Status");
    return qdbus_cast<QString>(var);
}

quint16 QOfonoNetworkRegistrationInterface::getLac()
{
    QVariant var = getProperty("LocationAreaCode");
    return qdbus_cast<quint16>(var);
}

quint32 QOfonoNetworkRegistrationInterface::getCellId()
{
    QVariant var = getProperty("CellId");
    return qdbus_cast<quint32>(var);
}

QString QOfonoNetworkRegistrationInterface::getTechnology()
{
    QVariant var = getProperty("Technology");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkRegistrationInterface::getOperatorName()
{
    QVariant var = getProperty("Name");
    return qdbus_cast<QString>(var);
}

int QOfonoNetworkRegistrationInterface::getSignalStrength()
{
    QVariant var = getProperty("Strength");
    return qdbus_cast<int>(var);
}

QList<QDBusObjectPath> QOfonoNetworkRegistrationInterface::getOperators()
{
    QDBusReply<QOfonoPropertyMap> reply = call(QLatin1String("GetOperators"));
    QList<QDBusObjectPath> operators;
    if (reply.isValid()) {
        foreach (const QOfonoProperties &property, reply.value())
            operators << property.path;
    }
    return operators;
}

void QOfonoNetworkRegistrationInterface::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(propertyChangedContext(QString,QString,QDBusVariant))) {
        QOfonoDBusHelper *helper = new QOfonoDBusHelper(this);
        QDBusConnection::systemBus().connect(QLatin1String(OFONO_SERVICE),
                                             path(),
                                             QLatin1String(OFONO_NETWORK_REGISTRATION_INTERFACE),
                                             QLatin1String("PropertyChanged"),
                                             helper, SLOT(propertyChanged(QString,QDBusVariant)));
        connect(helper, SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)));
    }
}

void QOfonoNetworkRegistrationInterface::disconnectNotify(const char */*signal*/)
{
    // TODO: disconnect the signal
}

QVariant QOfonoNetworkRegistrationInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QOfonoNetworkOperatorInterface::QOfonoNetworkOperatorInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE), dbusPathName,
                             OFONO_NETWORK_OPERATOR_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QOfonoNetworkOperatorInterface::~QOfonoNetworkOperatorInterface()
{
}

QString QOfonoNetworkOperatorInterface::getStatus()
{
    QVariant var = getProperty("Status");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkOperatorInterface::getMcc()
{
    QVariant var = getProperty("MobileCountryCode");
    return qdbus_cast<QString>(var);
}

QString QOfonoNetworkOperatorInterface::getMnc()
{
    QVariant var = getProperty("MobileNetworkCode");
    return qdbus_cast<QString>(var);
}

QStringList QOfonoNetworkOperatorInterface::getTechnologies()
{
    QVariant var = getProperty("Technologies");
    return qdbus_cast<QStringList>(var);
}

QVariant QOfonoNetworkOperatorInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QOfonoSimInterface::QOfonoSimInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE), dbusPathName,
                             OFONO_SIM_MANAGER_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QOfonoSimInterface::~QOfonoSimInterface()
{
}

bool QOfonoSimInterface::isPresent()
{
    QVariant var = getProperty("Present");
    return qdbus_cast<bool>(var);
}

QString QOfonoSimInterface::getHomeMcc()
{
    QVariant var = getProperty("MobileCountryCode");
    return qdbus_cast<QString>(var);
}

QString QOfonoSimInterface::getHomeMnc()
{
    QVariant var = getProperty("MobileNetworkCode");
    return qdbus_cast<QString>(var);
}

QString QOfonoSimInterface::pinRequired()
{
    QVariant var = getProperty("PinRequired");
    return qdbus_cast<QString>(var);
}

QString QOfonoSimInterface::getImsi()
{
    QVariant var = getProperty("SubscriberIdentity");
    return qdbus_cast<QString>(var);
}

QVariant QOfonoSimInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

QOfonoConnectionManagerInterface::QOfonoConnectionManagerInterface(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface(QLatin1String(OFONO_SERVICE), dbusPathName,
                             OFONO_CONNECTION_MANAGER_INTERFACE, QDBusConnection::systemBus(), parent)
{
}

QOfonoConnectionManagerInterface::~QOfonoConnectionManagerInterface()
{
}

QString QOfonoConnectionManagerInterface::bearer()
{
    QVariant var = getProperty("Bearer");
    return qdbus_cast<QString>(var);
}

QVariant QOfonoConnectionManagerInterface::getProperty(const QString &property)
{
    QVariant value;

    if (isValid()) {
        QDBusReply<QVariantMap> reply = call(QLatin1String("GetProperties"));
        value = reply.value().value(property);
    }

    return value;
}

#include "moc_qofonoservice_linux_p.cpp"

QTM_END_NAMESPACE

#endif // QT_NO_DBUS
