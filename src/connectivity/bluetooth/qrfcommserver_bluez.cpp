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

#include "qrfcommserver.h"
#include "qrfcommserver_p.h"
#include "qbluetoothsocket.h"

#include <QtCore/QSocketNotifier>

#include <QtCore/QDebug>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

QTM_BEGIN_NAMESPACE

static inline void convertAddress(quint64 from, quint8 (&to)[6])
{
    to[0] = (from >> 0) & 0xff;
    to[1] = (from >> 8) & 0xff;
    to[2] = (from >> 16) & 0xff;
    to[3] = (from >> 24) & 0xff;
    to[4] = (from >> 32) & 0xff;
    to[5] = (from >> 40) & 0xff;
}

QRfcommServerPrivate::QRfcommServerPrivate()
:   maxPendingConnections(1), socketNotifier(0)
{
    socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);
}

QRfcommServerPrivate::~QRfcommServerPrivate()
{
    delete socketNotifier;

    delete socket;
}

void QRfcommServerPrivate::_q_newConnection()
{
    // disable socket notifier until application calls nextPendingConnection().
    socketNotifier->setEnabled(false);

    emit q_ptr->newConnection();
}

void QRfcommServer::close()
{
    Q_D(QRfcommServer);

    d->socket->close();
}

bool QRfcommServer::listen(const QBluetoothAddress &address, quint16 port)
{
    Q_D(QRfcommServer);

    int sock = d->socket->socketDescriptor();
    if (sock < 0)
        return false;

    sockaddr_rc addr;

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = port;

    if (!address.isNull())
        convertAddress(address.toUInt64(), addr.rc_bdaddr.b);
    else
        convertAddress(Q_UINT64_C(0), addr.rc_bdaddr.b);

    if (::bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(sockaddr_rc)) < 0)
        return false;

    if (::listen(sock, d->maxPendingConnections) < 0)
        return false;

    d->socket->setSocketState(QBluetoothSocket::ListeningState);

    if (!d->socketNotifier) {
        d->socketNotifier = new QSocketNotifier(d->socket->socketDescriptor(),
                                                QSocketNotifier::Read);
        connect(d->socketNotifier, SIGNAL(activated(int)), this, SLOT(_q_newConnection()));
    }

    return true;
}

void QRfcommServer::setMaxPendingConnections(int numConnections)
{
    Q_D(QRfcommServer);

    if (d->socket->state() == QBluetoothSocket::UnconnectedState)
        d->maxPendingConnections = numConnections;
}

bool QRfcommServer::hasPendingConnections() const
{
    Q_D(const QRfcommServer);

    if (!d || !d->socketNotifier)
        return false;

    // if the socket notifier is disable there is a pending connection waiting for us to accept.
    return !d->socketNotifier->isEnabled();
}

QBluetoothSocket *QRfcommServer::nextPendingConnection()
{
    Q_D(QRfcommServer);

    if (!hasPendingConnections())
        return 0;

    sockaddr_rc addr;
    socklen_t length = sizeof(sockaddr_rc);

    int pending = ::accept(d->socket->socketDescriptor(),
                           reinterpret_cast<sockaddr *>(&addr), &length);
    if (pending >= 0) {
        QBluetoothSocket *newSocket = new QBluetoothSocket;
        newSocket->setSocketDescriptor(pending);

        d->socketNotifier->setEnabled(true);

        return newSocket;
    } else {
        qDebug() << "Hmm, could have sworn there was a connection waiting to be accepted!";
        d->socketNotifier->setEnabled(true);
    }

    return 0;
}

QBluetoothAddress QRfcommServer::serverAddress() const
{
    Q_D(const QRfcommServer);

    return d->socket->localAddress();
}

quint16 QRfcommServer::serverPort() const
{
    Q_D(const QRfcommServer);

    return d->socket->localPort();
}

QTM_END_NAMESPACE
