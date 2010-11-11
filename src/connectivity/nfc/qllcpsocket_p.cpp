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

#include "qllcpsocket_p.h"

QTM_BEGIN_NAMESPACE

QLlcpSocketPrivate::QLlcpSocketPrivate()
{
}

void QLlcpSocketPrivate::connectToService(QNearFieldTarget *target, const QString &serviceUri)
{
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);
}

void QLlcpSocketPrivate::disconnectFromService()
{
}

bool QLlcpSocketPrivate::bind(quint8 port)
{
    Q_UNUSED(port);

    return false;
}

bool QLlcpSocketPrivate::hasPendingDatagrams() const
{
    return false;
}

qint64 QLlcpSocketPrivate::pendingDatagramSize() const
{
    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    Q_UNUSED(data);
    Q_UNUSED(size);

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
    Q_UNUSED(datagram);

    return -1;
}

qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget *target, quint8 *port)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    Q_UNUSED(target);
    Q_UNUSED(port);

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(data);
    Q_UNUSED(size);
    Q_UNUSED(target);
    Q_UNUSED(port);

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(datagram);
    Q_UNUSED(target);
    Q_UNUSED(port);

    return -1;
}

QLlcpSocket::Error QLlcpSocketPrivate::error() const
{
    return QLlcpSocket::UnknownSocketError;
}

qint64 QLlcpSocketPrivate::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return -1;
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return -1;
}

QTM_END_NAMESPACE
