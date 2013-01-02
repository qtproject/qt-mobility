/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
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
