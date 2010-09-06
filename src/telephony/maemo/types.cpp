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

#include "maemo/types.h"
#include "maemo/interfaces/itypes.h"

namespace DBus
{
    void registerTypes()
    {
        qDBusRegisterMetaType<DBus::SUSocketAddress>();

        DBus::Interfaces::_registerTypes();
    }

    bool operator==(const SUSocketAddress& v1, const SUSocketAddress& v2)
    {
        return ((v1.address == v2.address)
            && (v1.port == v2.port)
            );
    }

    QDBusArgument& operator<<(QDBusArgument& arg, const SUSocketAddress& val)
    {
        arg.beginStructure();
        arg << val.address << val.port;
        arg.endStructure();
        return arg;
    }

    const QDBusArgument& operator>>(const QDBusArgument& arg, SUSocketAddress& val)
    {
        arg.beginStructure();
        arg >> val.address >> val.port;
        arg.endStructure();
        return arg;
    }
}

template<> inline DBus::Interfaces::SocketAddressIPv4 qdbus_cast<DBus::Interfaces::SocketAddressIPv4>(const QDBusArgument &arg,
DBus::Interfaces::SocketAddressIPv4 *)
{
    if (arg.currentSignature() == QLatin1String("(su)")) {
        DBus::SUSocketAddress saddr = qdbus_cast<DBus::SUSocketAddress>(arg);
        DBus::Interfaces::SocketAddressIPv4 addr;
        addr.address = saddr.address;
        addr.port = saddr.port;
        return addr;
    }
    else if (arg.currentSignature() == QLatin1String("(sq)")) {
        DBus::Interfaces::SocketAddressIPv4 item;
        arg >> item;
        return item;
    }
    else {
        return DBus::Interfaces::SocketAddressIPv4();
    }
}

template<> inline DBus::Interfaces::SocketAddressIPv4 qdbus_cast<DBus::Interfaces::SocketAddressIPv4>(const QVariant &v, DBus::Interfaces::SocketAddressIPv4 *)
{
    int id = v.userType();
    if (id == qMetaTypeId<QDBusArgument>()) {
        QDBusArgument arg = qvariant_cast<QDBusArgument>(v);

        if (arg.currentSignature() == QLatin1String("(su)")) {
            DBus::SUSocketAddress saddr = qdbus_cast<DBus::SUSocketAddress>(arg);
            DBus::Interfaces::SocketAddressIPv4 addr;
            addr.address = saddr.address;
            addr.port = saddr.port;
            return addr;
        }
        else if (arg.currentSignature() == QLatin1String("(sq)")) {
            DBus::Interfaces::SocketAddressIPv4 item;
            arg >> item;
            return item;
        }
        else {
            return DBus::Interfaces::SocketAddressIPv4();
        }
    }
    else
        return qvariant_cast<DBus::Interfaces::SocketAddressIPv4>(v);
}

template<> inline DBus::Interfaces::SocketAddressIPv6 qdbus_cast<DBus::Interfaces::SocketAddressIPv6>(const QDBusArgument &arg,
DBus::Interfaces::SocketAddressIPv6 *)
{
    if (arg.currentSignature() == QLatin1String("(su)")) {
        DBus::SUSocketAddress saddr = qdbus_cast<DBus::SUSocketAddress>(arg);
        DBus::Interfaces::SocketAddressIPv6 addr;
        addr.address = saddr.address;
        addr.port = saddr.port;
        return addr;
    }
    else if (arg.currentSignature() == QLatin1String("(sq)")) {
        DBus::Interfaces::SocketAddressIPv6 item;
        arg >> item;
        return item;
    }
    else {
        return DBus::Interfaces::SocketAddressIPv6();
    }
}

template<> inline DBus::Interfaces::SocketAddressIPv6 qdbus_cast<DBus::Interfaces::SocketAddressIPv6>(const QVariant &v, DBus::Interfaces::SocketAddressIPv6 *)
{
    int id = v.userType();
    if (id == qMetaTypeId<QDBusArgument>()) {
        QDBusArgument arg = qvariant_cast<QDBusArgument>(v);

        if (arg.currentSignature() == QLatin1String("(su)")) {
            DBus::SUSocketAddress saddr = qdbus_cast<DBus::SUSocketAddress>(arg);
            DBus::Interfaces::SocketAddressIPv6 addr;
            addr.address = saddr.address;
            addr.port = saddr.port;
            return addr;
        }
        else if (arg.currentSignature() == QLatin1String("(sq)")) {
            DBus::Interfaces::SocketAddressIPv6 item;
            arg >> item;
            return item;
        }
        else {
            return DBus::Interfaces::SocketAddressIPv6();
        }
    }
    else
        return qvariant_cast<DBus::Interfaces::SocketAddressIPv6>(v);
}
