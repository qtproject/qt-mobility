/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qbluetoothsocket_p.h"
#include "qbluetoothlocaldevice.h"
#include "symbian/utils_symbian_p.h"

#include <QTimer>
#include <QCoreApplication>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QRfcommServerPrivate::QRfcommServerPrivate()
: pendingSocket(0), maxPendingConnections(1), securityFlags(QBluetooth::NoSecurity)
{
    socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);
    ds = socket->d_ptr;
    ds->iSocket->SetNotifier(*this);
}

QRfcommServerPrivate::~QRfcommServerPrivate()
{
    delete pendingSocket;

    delete socket;
}

void QRfcommServer::close()
{
    Q_D(QRfcommServer);

    d->socket->close();

    // force active object (socket) to run and shutdown socket.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

bool QRfcommServer::listen(const QBluetoothAddress &address, quint16 port)
{
    Q_D(QRfcommServer);

    TRfcommSockAddr addr;
    if (!address.isNull())
        addr.SetBTAddr(TBTDevAddr(address.toUInt64()));
    else
        addr.SetBTAddr(TBTDevAddr(Q_UINT64_C(0)));

    if (port == 0)
        addr.SetPort(KRfcommPassiveAutoBind);
    else
        addr.SetPort(port);

    TBTServiceSecurity security;
    switch (d->securityFlags) {
        case QBluetooth::Authentication:
            security.SetAuthentication(ETrue);
            break;
        case QBluetooth::Authorization:
            security.SetAuthorisation(ETrue);
            break;
        case QBluetooth::Encryption:
            security.SetEncryption(ETrue);
            break;
        case QBluetooth::NoSecurity:
        default:
            break;
    }
    addr.SetSecurity(security);
    d->ds->iSocket->Bind(addr);
    d->socket->setSocketState(QBluetoothSocket::BoundState);

    d->ds->iSocket->Listen(d->maxPendingConnections);

    d->pendingSocket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);

    QBluetoothSocketPrivate *pd = d->pendingSocket->d_ptr;

    pd->ensureBlankNativeSocket();

    if (d->ds->iSocket->Accept(*pd->iBlankSocket) == KErrNone)
        d->socket->setSocketState(QBluetoothSocket::ListeningState);
    else
        d->socket->close();

    return d->socket->state() == QBluetoothSocket::ListeningState;
}

void QRfcommServer::setMaxPendingConnections(int numConnections)
{
    Q_D(QRfcommServer);

    if (d->socket->state() == QBluetoothSocket::UnconnectedState)
        d->maxPendingConnections = numConnections;
}

QBluetoothAddress QRfcommServer::serverAddress() const
{
    Q_D(const QRfcommServer);

    if (d->socket->state() == QBluetoothSocket::UnconnectedState)
        return QBluetoothAddress();

    return d->ds->localAddress();
}

quint16 QRfcommServer::serverPort() const
{
    Q_D(const QRfcommServer);

    if (d->socket->state() == QBluetoothSocket::UnconnectedState)
        return 0;

    return d->ds->iSocket->LocalPort();
}

bool QRfcommServer::hasPendingConnections() const
{
    Q_D(const QRfcommServer);

    return !d->activeSockets.isEmpty();
}

QBluetoothSocket *QRfcommServer::nextPendingConnection()
{
    Q_D(QRfcommServer);

    if (d->activeSockets.isEmpty())
        return 0;

    QBluetoothSocket *next = d->activeSockets.takeFirst();
    QBluetoothSocketPrivate *n = next->d_ptr;

    n->startReceive();

    return next;
}

void QRfcommServerPrivate::HandleAcceptCompleteL(TInt aErr)
{
    Q_Q(QRfcommServer);

    if (aErr == KErrNone) {
        pendingSocket->setSocketState(QBluetoothSocket::ConnectedState);
        activeSockets.append(pendingSocket);

        QBluetoothSocketPrivate *pd = pendingSocket->d_ptr;

        if (!pd->iBlankSocket)
            pd->ensureBlankNativeSocket();

        pd->iSocket->Accept(*pd->iBlankSocket);
        emit q->newConnection();
    } else if (aErr == KErrCancel) {
        // server is closing
        delete pendingSocket;
        pendingSocket = 0;
        socket->setSocketState(QBluetoothSocket::BoundState);
    } else {
        qDebug() << __PRETTY_FUNCTION__ << aErr;
        return;
    }
}

void QRfcommServerPrivate::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification &aEventNotification)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QRfcommServerPrivate::HandleConnectCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QRfcommServerPrivate::HandleIoctlCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QRfcommServerPrivate::HandleReceiveCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QRfcommServerPrivate::HandleSendCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QRfcommServerPrivate::HandleShutdownCompleteL(TInt aErr)
{
    if (aErr == KErrNone)
        socket->setSocketState(QBluetoothSocket::UnconnectedState);
    else
        qDebug() << __PRETTY_FUNCTION__ << aErr;
}

QTM_END_NAMESPACE
