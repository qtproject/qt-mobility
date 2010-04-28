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
static quint8 baseUuid[16] = { 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00,
                               0x10, 0x00,
                               0x80, 0x00,
                               0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB
};

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
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    d->uuidType = QBluetoothUuidPrivate::NullUuid;
}

/*!
    Constructs a new Bluetooth UUID from the protocol UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(ProtocolUuid uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    d->uuidType = QBluetoothUuidPrivate::Uuid16;
    d->uuid16 = uuid;
}

/*!
    Constructs a new Bluetooth UUID from the service class UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(ServiceClassUuid uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    d->uuidType = QBluetoothUuidPrivate::Uuid16;
    d->uuid16 = uuid;
}

/*!
    Constructs a new Bluetooth UUID from the 16 bit UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(quint16 uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    d->uuidType = QBluetoothUuidPrivate::Uuid16;
    d->uuid16 = uuid;
}

/*!
    Constructs a new Bluetooth UUID from the 32 bit UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(quint32 uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    if (uuid & 0xFFFF0000) {
        d->uuidType = QBluetoothUuidPrivate::Uuid32;
        d->uuid32 = uuid;
    } else {
        d->uuidType = QBluetoothUuidPrivate::Uuid16;
        d->uuid16 = uuid;
    }
}

/*!
    Constructs a new Bluetooth UUID from the 128 bit UUID \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(quint128 uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    d->uuidType = QBluetoothUuidPrivate::Uuid128;
    memcpy(d->uuid128.data, uuid.data, 16);
}

/*!
    Constructs a new Bluetooth UUID from the string \a uuid.

    The string must be in the form XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.
*/
QBluetoothUuid::QBluetoothUuid(const QString &uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    Q_D(QBluetoothUuid);

    d->uuidType = QBluetoothUuidPrivate::NullUuid;

    if (uuid.length() != 36)
        return;

    bool ok;

    QStringList segments = uuid.split('-');

    quint32 s1 = htonl(segments[0].toUInt(&ok, 16));
    if (!ok)
        return;

    quint16 s2 = htons(segments[1].toUShort(&ok, 16));
    if (!ok)
        return;

    quint16 s3 = htons(segments[2].toUShort(&ok, 16));
    if (!ok)
        return;

    quint16 s4 = htons(segments[3].toUShort(&ok, 16));

    quint8 s5[6];
    for (int i = 0; i < 6; ++i) {
        s5[i] = segments[4].mid(i*2, 2).toUShort(&ok, 16);
        if (!ok)
            return;
    }

    memcpy(&d->uuid128.data[0], &s1, 4);
    memcpy(&d->uuid128.data[4], &s2, 2);
    memcpy(&d->uuid128.data[6], &s3, 2);
    memcpy(&d->uuid128.data[8], &s4, 2);
    memcpy(&d->uuid128.data[10], s5, 6);

    if (memcmp(&d->uuid128.data[4], &baseUuid[4], 12) == 0) {
        // base UUID, convertible to 16 bit or 32 bit UUID
        quint32 uuid32 = ntohl(s1);
        if (uuid32 & 0xFFFF0000) {
            d->uuidType = QBluetoothUuidPrivate::Uuid32;
            d->uuid32 = uuid32;
        } else {
            d->uuidType = QBluetoothUuidPrivate::Uuid16;
            d->uuid16 = uuid32;
        }
    } else {
        d->uuidType = QBluetoothUuidPrivate::Uuid128;
    }
}

/*!
    Constructs a new Bluetooth UUID that is a copy of \a uuid.
*/
QBluetoothUuid::QBluetoothUuid(const QBluetoothUuid &uuid)
: d_ptr(new QBluetoothUuidPrivate)
{
    *this = uuid;
}

/*!
    Destroys the Bluetooth UUID.
*/
QBluetoothUuid::~QBluetoothUuid()
{
    delete d_ptr;
}

/*!
    Returns true if this is a null UUID, otherwise returns false.
*/
bool QBluetoothUuid::isNull() const
{
    Q_D(const QBluetoothUuid);

    return d->uuidType == QBluetoothUuidPrivate::NullUuid;
}

/*!
    Assigns a copy of \a other to this Bluetooth UUID.
*/
QBluetoothUuid &QBluetoothUuid::operator=(const QBluetoothUuid &other)
{
    Q_D(QBluetoothUuid);

    d->uuidType = other.d_func()->uuidType;

    switch (d->uuidType) {
    case QBluetoothUuidPrivate::NullUuid:
        break;
    case QBluetoothUuidPrivate::Uuid16:
        d->uuid16 = other.d_func()->uuid16;
        break;
    case QBluetoothUuidPrivate::Uuid32:
        d->uuid32 = other.d_func()->uuid32;
        break;
    case QBluetoothUuidPrivate::Uuid128:
        memcpy(d->uuid128.data, other.d_func()->uuid128.data, 16);
        break;
    }

    return *this;
}

/*!
    Returns true if this Bluetooth UUID is the same as \a other, otherwise returns false.
*/
bool QBluetoothUuid::operator==(const QBluetoothUuid &other) const
{
    Q_D(const QBluetoothUuid);

    if (d->uuidType == other.d_func()->uuidType) {
        // do direct comparison
        switch (d->uuidType) {
        case QBluetoothUuidPrivate::NullUuid:
            return true;
        case QBluetoothUuidPrivate::Uuid16:
            return d->uuid16 == other.d_func()->uuid16;
        case QBluetoothUuidPrivate::Uuid32:
            return d->uuid32 == other.d_func()->uuid32;
        case QBluetoothUuidPrivate::Uuid128:
            return memcmp(d->uuid128.data, other.d_func()->uuid128.data, 16) == 0;
        default:
            return false;
        }
    }

    if (d->uuidType < other.d_func()->uuidType) {
        bool ok;
        switch (other.d_func()->uuidType) {
        case QBluetoothUuidPrivate::NullUuid:
            return false;
        case QBluetoothUuidPrivate::Uuid16:
            return (toUInt16(&ok) == other.d_func()->uuid16) && ok;
        case QBluetoothUuidPrivate::Uuid32:
            return (toUInt32(&ok) == other.d_func()->uuid32) && ok;
        case QBluetoothUuidPrivate::Uuid128:
            return memcmp(toUInt128().data, other.d_func()->uuid128.data, 16) == 0;
        default:
            return false;
        }
    }

    if (d->uuidType > other.d_func()->uuidType) {
        bool ok;
        switch (d->uuidType) {
        case QBluetoothUuidPrivate::NullUuid:
            return false;
        case QBluetoothUuidPrivate::Uuid16:
            return (d->uuid16 == other.toUInt16(&ok)) && ok;
        case QBluetoothUuidPrivate::Uuid32:
            return (d->uuid32 == other.toUInt32(&ok)) && ok;
        case QBluetoothUuidPrivate::Uuid128:
            return memcmp(d->uuid128.data, other.toUInt128().data, 16) == 0;
        default:
            return false;
        }
    }

    return false;
}

/*!
    Returns the minimum size in bytes that this UUID can be represented in.  For non-null UUIDs 2,
    4 or 16 is returned.  0 is returned for null UUIDs.

    \sa isNull(), toUInt16(), toUInt32(), toUInt128()
*/
int QBluetoothUuid::minimumSize() const
{
    Q_D(const QBluetoothUuid);

    switch (d->uuidType) {
    case QBluetoothUuidPrivate::NullUuid:
        return 0;
    case QBluetoothUuidPrivate::Uuid16:
        return 2;
    case QBluetoothUuidPrivate::Uuid32:
        return 4;
    case QBluetoothUuidPrivate::Uuid128:
        return 16;
    default:
        return 0;
    }
}

/*!
    Returns the 16 bit representation of this UUID. If \a ok is passed it is set to true if the
    conversion is possible otherwise it is set to false. The return value is undefined if \a ok is
    set to false.
 */
quint16 QBluetoothUuid::toUInt16(bool *ok) const
{
    Q_D(const QBluetoothUuid);

    switch (d->uuidType) {
    case QBluetoothUuidPrivate::NullUuid:
        if (ok)
            *ok = false;
        return 0;
    case QBluetoothUuidPrivate::Uuid16:
        if (ok)
            *ok = true;
        return d->uuid16;
    case QBluetoothUuidPrivate::Uuid32:
        if (d->uuid32 & 0xFFFF0000) {
            // high 16 bits not 0, cannot convert to 16 bit UUID
            if (ok)
                *ok = false;
            return 0;
        } else {
            // high 16 bits are all 0, can convert to 16 bit UUID
            if (ok)
                *ok = true;
            return d->uuid32;
        }
    case QBluetoothUuidPrivate::Uuid128:
        if (memcmp(&d->uuid128.data[4], &baseUuid[4], 12) == 0) {
            // base UUID, convertable to 16 bit or 32 bit UUID
            if (d->uuid128.data[0] == 0 && d->uuid128.data[1] == 0) {
                // 16 bit UUID
                if (ok)
                    *ok = true;
                quint16 tmp;
                memcpy(&tmp, &d->uuid128.data[2], 2);
                return ntohs(tmp);
            }
        }

        if (ok)
            *ok = false;
        return 0;
    default:
        if (ok)
            *ok = false;
        qDebug() << "Trying to convert unknown UUID to 16 bit UUID.";
        return 0;
    }
}

/*!
    Returns the 32 bit representation of this UUID. If \a ok is passed it is set to true if the
    conversion is possible otherwise it is set to false. The return value is undefined if \a ok is
    set to false.
*/
quint32 QBluetoothUuid::toUInt32(bool *ok) const
{
    Q_D(const QBluetoothUuid);

    switch (d->uuidType) {
    case QBluetoothUuidPrivate::NullUuid:
        if (ok)
            *ok = false;
        return 0;
    case QBluetoothUuidPrivate::Uuid16:
        if (ok)
            *ok = true;
        return d->uuid16;
    case QBluetoothUuidPrivate::Uuid32:
        if (ok)
            *ok = true;
        return d->uuid32;
    case QBluetoothUuidPrivate::Uuid128:
        if (memcmp(&d->uuid128.data[4], &baseUuid[4], 12) == 0) {
            // base UUID, convertible to 16 bit or 32 bit UUID
            if (d->uuid128.data[0] == 0 && d->uuid128.data[1] == 0) {
                // 16 bit UUID
                if (ok)
                    *ok = true;
                quint16 tmp;
                memcpy(&tmp, &d->uuid128.data[2], 2);
                return ntohs(tmp);
            } else {
                // 32 bit UUID
                if (ok)
                    *ok = true;
                quint32 tmp;
                memcpy(&tmp, &d->uuid128.data[0], 4);
                return ntohl(tmp);
            }
        }

        if (ok)
            *ok = false;
        return 0;
    default:
        if (ok)
            *ok = false;
        qDebug() << "Trying to convert unknown UUID to 32 bit UUID.";
        return 0;
    }
}

/*!
    Returns the 128 bit representation of this UUID.
*/
quint128 QBluetoothUuid::toUInt128() const
{
    Q_D(const QBluetoothUuid);

    quint128 uuid;
    memcpy(uuid.data, baseUuid, 16);

    switch (d->uuidType) {
    case QBluetoothUuidPrivate::NullUuid:
        return uuid;
    case QBluetoothUuidPrivate::Uuid16: {
        quint16 tmp = htons(d->uuid16);
        memcpy(&uuid.data[2], &tmp, 2);
        return uuid;
    }
    case QBluetoothUuidPrivate::Uuid32: {
        quint32 tmp = htonl(d->uuid32);
        memcpy(&uuid.data[0], &tmp, 4);
        return uuid;
    }
    case QBluetoothUuidPrivate::Uuid128:
        return d->uuid128;
    default:
        qDebug() << "Trying to convert unknown UUID to 128 bit UUID.";
        return uuid;
    }
}

/*!
    Returns the 128 bit representation of this UUID as a string.

    The returned string is in the format XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX, where
    X is a hexadecimal number.
*/
QString QBluetoothUuid::toString() const
{
    Q_D(const QBluetoothUuid);

    QByteArray uuid;

    switch (d->uuidType) {
    case QBluetoothUuidPrivate::NullUuid:
        return QString();
    case QBluetoothUuidPrivate::Uuid16: {
        uuid = QByteArray(reinterpret_cast<const char *>(baseUuid), 16);
        quint16 tmp = htons(d->uuid16);
        memcpy(uuid.data() + 2, reinterpret_cast<const char *>(&tmp), 2);
        break;
    }
    case QBluetoothUuidPrivate::Uuid32: {
        uuid = QByteArray(reinterpret_cast<const char *>(baseUuid), 16);
        quint32 tmp = htonl(d->uuid32);
        memcpy(uuid.data(), reinterpret_cast<const char *>(&tmp), 4);
        break;
    }
    case QBluetoothUuidPrivate::Uuid128:
        uuid = QByteArray(reinterpret_cast<const char *>(d->uuid128.data), 16);
        break;
    }

    QString hex = uuid.toHex();
    hex.insert(8, '-');
    hex.insert(13, '-');
    hex.insert(18, '-');
    hex.insert(23, '-');

    return hex;
}

QTM_END_NAMESPACE
