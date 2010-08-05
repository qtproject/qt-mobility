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


#include "qcontactlesssocket.h"

/*!
    \class QContactlessSocket
    \brief The QContactlessSocket class provides an NFC LLCP socket.

    NFC LLCP protocol is a peer-to-peer communication protocol between two NFC complient devices.
*/

/*!
    Construct a new unconnected contactless socket with \a parent.
*/
QContactlessSocket::QContactlessSocket(QObject *parent)
:   QIODevice(parent)
{
}

/*!
    Destroys the contactless socket.
*/
QContactlessSocket::~QContactlessSocket()
{
}

/*!
    Connects to the service identified by the URI \a serviceUri on \a target.
*/
void QContactlessSocket::connectToService(const QContactlessTarget &target,
                                          const QString &serviceUri)
{
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);
}

/*!
    Disconnects the socket.
*/
void QContactlessSocket::disconnectFromService()
{
}

/*!
    Returns true if at least one datagram (service data units) is waiting to be read; otherwise
    returns false.

    \sa pendingDatagramSize(), readDatagram()
*/
bool QContactlessSocket::hasPendingDatagrams() const
{
    return false;
}

/*!
    Returns the size of the first pending datagram (service data unit). If there is no datagram
    available, this function returns -1.

    \sa hasPendingDatagrams(), readDatagram()
*/
qint64 QContactlessSocket::pendingDatagramSize() const
{
    return -1;
}

/*!
    Receives a datagram no larger than \a maxSize bytes and stores it in \a data. The sender's
    details are stored in \a target and \a serviceUri (unless the pointers are 0).

    Returns the size of the datagram on success; otherwise returns -1.

    If maxSize is too small, the rest of the datagram will be lost. To avoid loss of data, call
    pendingDatagramSize() to determine the size of the pending datagram before attempting to read
    it. If maxSize is 0, the datagram will be discarded.

    \sa writeDatagram(), hasPendingDatagrams(), pendingDatagramSize()
*/
qint64 QContactlessSocket::readDatagram(char *data, qint64 maxSize,
                                        QContactlessTarget *target, QString *serviceUri)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);

    return -1;
}

/*!
    Sends the datagram at \a data of size \a size to the service identified by the URI
    \a serviceUri on \a target. Returns the number of bytes sent on success; otherwise returns -1.

    \sa readDatagram()
*/
qint64 QContactlessSocket::writeDatagram(const char *data, qint64 size,
                                         const QContactlessTarget &target,
                                         const QString &serviceUri)
{
    Q_UNUSED(data);
    Q_UNUSED(size);
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);

    return -1;
}

/*!
    \overload

    Sends the datagram \a datagram to the service identified by the URI \a serviceUri on \a target.
*/
qint64 QContactlessSocket::writeDatagram(const QByteArray &datagram,
                                         const QContactlessTarget &target,
                                         const QString &serviceUri)
{
    Q_UNUSED(datagram);
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);

    return -1;
}

/*!
    \internal
*/
qint64 QContactlessSocket::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return 0;
}

/*!
    \internal
*/
qint64 QContactlessSocket::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}
