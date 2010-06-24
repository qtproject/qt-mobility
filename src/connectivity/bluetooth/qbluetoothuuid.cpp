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

#include "qbluetoothuuid.h"
#include "qbluetoothuuid_p.h"

#include <QStringList>

#include <QDebug>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

QTM_BEGIN_NAMESPACE

// Bluetooth base UUID 00000000-0000-1000-8000-00805F9B34FB
static QUuid baseUuid(QLatin1String("{00000000-0000-1000-8000-00805F9B34FB}"));

/*!
    \class QBluetoothUuid
    \brief The QBluetoothUuid class provides a Bluetooth UUID.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity
*/

/*!
    \enum QBluetoothUuid::ProtocolUuid

    This enum is a convienience type for Bluetooth protocol UUIDs. Values of this type will be
    implicitly converted into a QBluetoothUuid when necessary.

    \value Sdp      SDP protocol UUID.
    \value Udp      UDP protocol UUID.
    \value Rfcomm   RFCOMM protocol UUID.
    \value Tcp      TCP protocol UUID.
    \value Obex     OBEX protocol UUID.
    \value Ip       IP protocol UUID.
    \value Ftp      FTP protocol UUID.
    \value Http     HTTP protocol UUID.
    \value L2cap    L2CAP protocol UUID.
*/

/*!
    \enum QBluetoothUuid::ServiceClassUuid

    This enum is a convienience type for Bluetooth service class UUIDs. Values of this type will be
    implicitly converted into a QBluetoothUuid when necessary.

    \value PublicBrowseGroup    Public browse group service class. Services which have the public
                                browse group in their
                                \l {QBluetoothServiceInfo::BrowseGroupList}{browse group list} are
                                discoverable by remote devices.
    \value ObexObjectPush       OBEX object push service UUID.
*/

/*!
    Constructs a new null Bluetooth UUID.
*/
QBluetoothUuid::QBluetoothUuid()
{
}

/*!
    Constructs a new Bluetooth UUID from the protocol UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(ProtocolUuid uuid)
:   QUuid(uuid, baseUuid.data2, baseUuid.data3, baseUuid.data4[0], baseUuid.data4[1],
          baseUuid.data4[2], baseUuid.data4[3], baseUuid.data4[4], baseUuid.data4[5],
          baseUuid.data4[6], baseUuid.data4[7])
{
}

/*!
    Constructs a new Bluetooth UUID from the service class UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(ServiceClassUuid uuid)
:   QUuid(uuid, baseUuid.data2, baseUuid.data3, baseUuid.data4[0], baseUuid.data4[1],
          baseUuid.data4[2], baseUuid.data4[3], baseUuid.data4[4], baseUuid.data4[5],
          baseUuid.data4[6], baseUuid.data4[7])
{
}

/*!
    Constructs a new Bluetooth UUID from the 16 bit UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(quint16 uuid)
:   QUuid(uuid, baseUuid.data2, baseUuid.data3, baseUuid.data4[0], baseUuid.data4[1],
          baseUuid.data4[2], baseUuid.data4[3], baseUuid.data4[4], baseUuid.data4[5],
          baseUuid.data4[6], baseUuid.data4[7])
{
}

/*!
    Constructs a new Bluetooth UUID from the 32 bit UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(quint32 uuid)
:   QUuid(uuid, baseUuid.data2, baseUuid.data3, baseUuid.data4[0], baseUuid.data4[1],
          baseUuid.data4[2], baseUuid.data4[3], baseUuid.data4[4], baseUuid.data4[5],
          baseUuid.data4[6], baseUuid.data4[7])
{
}

/*!
    Constructs a new Bluetooth UUID from the 128 bit UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(quint128 uuid)
{
    quint32 tmp32;
    memcpy(&tmp32, &uuid.data[0], 4);
    data1 = ntohl(tmp32);

    quint16 tmp16;
    memcpy(&tmp16, &uuid.data[4], 2);
    data2 = ntohs(tmp16);

    memcpy(&tmp16, &uuid.data[6], 2);
    data3 = ntohs(tmp16);

    memcpy(data4, &uuid.data[8], 8);
}

/*!
    Constructs a new Bluetooth UUID from the string \a uuid.

    The string must be in the form XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.
*/
QBluetoothUuid::QBluetoothUuid(const QString &uuid)
:   QUuid(uuid)
{
}

/*!
    Constructs a new Bluetooth UUID that is a copy of \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(const QBluetoothUuid &uuid)
:   QUuid(uuid)
{
}

/*!
    Constructs a new Bluetooth UUID that is a copy of \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(const QUuid &uuid)
:   QUuid(uuid)
{
}

/*!
    Destroys the Bluetooth UUID.
*/
QBluetoothUuid::~QBluetoothUuid()
{
}

/*!
    Returns the minimum size in bytes that this UUID can be represented in.  For non-null UUIDs 2,
    4 or 16 is returned.  0 is returned for null UUIDs.

    \sa isNull(), toUInt16(), toUInt32(), toUInt128()
*/
int QBluetoothUuid::minimumSize() const
{
    if (data2 == baseUuid.data2 && data3 == baseUuid.data3 &&
        memcmp(data4, baseUuid.data4, 8) == 0) {
        // 16 or 32 bit Bluetooth UUID
        if (data1 & 0xFFFF0000)
            return 4;
        else
            return 2;
    }

    if (isNull())
        return 0;

    return 16;
}

/*!
    Returns the 16 bit representation of this UUID. If \a ok is passed it is set to true if the
    conversion is possible otherwise it is set to false. The return value is undefined if \a ok is
    set to false.
*/
quint16 QBluetoothUuid::toUInt16(bool *ok) const
{
    if (data1 & 0xFFFF0000 || data2 != baseUuid.data2 || data3 != baseUuid.data3 ||
        memcmp(data4, baseUuid.data4, 8) != 0) {
        // not convertable to 16 bit Bluetooth UUID.
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = true;

    return data1;
}

/*!
    Returns the 32 bit representation of this UUID. If \a ok is passed it is set to true if the
    conversion is possible otherwise it is set to false. The return value is undefined if \a ok is
    set to false.
*/
quint32 QBluetoothUuid::toUInt32(bool *ok) const
{
    if (data2 != baseUuid.data2 || data3 != baseUuid.data3 ||
        memcmp(data4, baseUuid.data4, 8) != 0) {
        // not convertable to 32 bit Bluetooth UUID.
        if (ok)
            *ok = false;
        return 0;
    }

    if (ok)
        *ok = true;

    return data1;
}

/*!
    Returns the 128 bit representation of this UUID.
*/
quint128 QBluetoothUuid::toUInt128() const
{
    quint128 uuid;

    quint32 tmp32 = htonl(data1);
    memcpy(&uuid.data[0], &tmp32, 4);

    quint16 tmp16 = htons(data2);
    memcpy(&uuid.data[4], &tmp16, 2);

    tmp16 = htons(data3);
    memcpy(&uuid.data[6], &tmp16, 2);

    memcpy(&uuid.data[8], data4, 8);

    return uuid;
}

/*!
    Returns true if \a other is equal to this Bluetooth UUID; otherwise returns false.
*/
bool QBluetoothUuid::operator==(const QBluetoothUuid &other) const
{
    return QUuid::operator==(other);
}

QTM_END_NAMESPACE
