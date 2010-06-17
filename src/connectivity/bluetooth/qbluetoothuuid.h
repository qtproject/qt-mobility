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

#ifndef QBLUETOOTHUUID_H
#define QBLUETOOTHUUID_H

#include "qmobilityglobal.h"

#include <QtCore/QtGlobal>
#include <QtCore/QMetaType>
#include <QtCore/QUuid>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

struct quint128
{
    quint8 data[16];
};

class Q_CONNECTIVITY_EXPORT QBluetoothUuid : public QUuid
{
public:
    enum ProtocolUuid {
        Sdp = 0x0001,
        Udp = 0x0002,
        Rfcomm = 0x0003,
        Tcp = 0x0004,
        TcsBin = 0x0005,
        TcsAt = 0x0006,
        Obex = 0x0008,
        Ip = 0x0009,
        Ftp = 0x000A,
        Http = 0x000C,
        Wsp = 0x000E,
        Bnep = 0x000F,
        Upnp = 0x0010,
        Hidp = 0x0011,
        HardcopyControlChannel = 0x0012,
        HardcopyDataChannel = 0x0014,
        HardcopyNotification = 0x0016,
        Avctp = 0x0017,
        Avdtp = 0x0019,
        Cmtp = 0x001B,
        UdiCPlain = 0x001D,
        McapControlChannel = 0x001E,
        McapDataChannel = 0x001F,
        L2cap = 0x0100,
    };

    enum ServiceClassUuid {
        PublicBrowseGroup = 0x1002,
        ObexObjectPush = 0x1105,
    };

    QBluetoothUuid();
    QBluetoothUuid(ProtocolUuid uuid);
    QBluetoothUuid(ServiceClassUuid uuid);
    explicit QBluetoothUuid(quint16 uuid);
    explicit QBluetoothUuid(quint32 uuid);
    explicit QBluetoothUuid(quint128 uuid);
    explicit QBluetoothUuid(const QString &uuid);
    QBluetoothUuid(const QBluetoothUuid &uuid);
    QBluetoothUuid(const QUuid &uuid);
    ~QBluetoothUuid();

    bool operator==(const QBluetoothUuid &other) const;

    int minimumSize() const;

    quint16 toUInt16(bool *ok = 0) const;
    quint32 toUInt32(bool *ok = 0) const;
    quint128 toUInt128() const;
};

QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QBluetoothUuid))

QT_END_HEADER

#endif
