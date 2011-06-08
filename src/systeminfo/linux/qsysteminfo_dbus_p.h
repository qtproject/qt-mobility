/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSYSTEMINFO_DBUS_P_H
#define QSYSTEMINFO_DBUS_P_H

#if !defined(QT_NO_HAL)
static bool halAvailable()
{
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("org.freedesktop.Hal");
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
    return false;
}
#endif // QT_NO_HAL

#if !defined(QT_NO_UDISKS)
static bool udisksAvailable()
{
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("org.freedesktop.UDisks");
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
    return false;
}
#endif // QT_NO_UDISKS

#if !defined(QT_NO_UPOWER)
static bool uPowerAvailable()
{
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("org.freedesktop.UPower");
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
    return false;
}
#endif // QT_NO_UPOWER

#if !defined(QT_NO_CONNMAN)
static bool connmanAvailable()
{
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("net.connman");
        if (reply.isValid() && reply.value())
            return reply.value();
    }
    return false;
}

static bool ofonoAvailable()
{
    if (QDBusConnection::systemBus().isConnected()) {
        QDBusReply<bool> reply = QDBusConnection::systemBus().interface()->isServiceRegistered("org.ofono");
        if (reply.isValid() && reply.value())
            return reply.value();
    }
    return false;
}
#endif // QT_NO_CONNMAN

#endif // QSYSTEMINFO_DBUS_P_H
