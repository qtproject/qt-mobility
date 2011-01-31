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

#include "ql2capserver.h"
#include "ql2capserver_p.h"
#include "qbluetoothsocket.h"
#include "qbluetoothsocket_p.h"
#include "utils_symbian_p.h"

#include <QTimer>
#include <QCoreApplication>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QL2capServerPrivate::QL2capServerPrivate()
: pending(false),maxPendingConnections(1)
{
    socket = new QBluetoothSocket(QBluetoothSocket::L2capSocket);
    ds = socket->d_ptr;
    ds->iSocket->SetNotifier(*this);
}

QL2capServerPrivate::~QL2capServerPrivate()
{
    delete pendingSocket;

    delete socket;
}

void QL2capServer::close()
{
    Q_D(QL2capServer);

    d->socket->close();

    // force active object (socket) to run and shutdown socket.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

bool QL2capServer::listen(const QBluetoothAddress &address, quint16 port)
{
    Q_D(QL2capServer);

    TL2CAPSockAddr addr;
    if (port == 0)
        addr.SetPort(KRfcommPassiveAutoBind);
    else
        addr.SetPort(port);

    TBTServiceSecurity security;
    addr.SetSecurity(security);
    d->ds->iSocket->Bind(addr);
    d->socket->setSocketState(QBluetoothSocket::BoundState);

    d->ds->iSocket->Listen(d->maxPendingConnections);

    d->pendingSocket = new QBluetoothSocket;

    QBluetoothSocketPrivate *pd = d->pendingSocket->d_ptr;
    pd->ensureBlankNativeSocket();
    if (d->ds->iSocket->Accept(*pd->iSocket) == KErrNone)
        d->socket->setSocketState(QBluetoothSocket::ListeningState);
    else
        d->socket->close();

    return d->socket->state() == QBluetoothSocket::ListeningState;
}

void QL2capServer::setMaxPendingConnections(int numConnections)
{
    Q_D(QL2capServer);

    if (d->socket->state() == QBluetoothSocket::UnconnectedState)
        d->maxPendingConnections = numConnections;
}

bool QL2capServer::hasPendingConnections() const
{
    Q_D(const QL2capServer);

    return !d->activeSockets.isEmpty();
}

QBluetoothSocket *QL2capServer::nextPendingConnection()
{
    Q_D(QL2capServer);

    if (d->activeSockets.isEmpty())
        return 0;

    QBluetoothSocket *next = d->activeSockets.takeFirst();
    QBluetoothSocketPrivate *n = next->d_ptr;

    n->startReceive();

    return next;
}

QBluetoothAddress QL2capServer::serverAddress() const
{
    Q_D(const QL2capServer);

    return d->socket->localAddress();
}

quint16 QL2capServer::serverPort() const
{
    Q_D(const QL2capServer);

    return d->socket->localPort();
}

void QL2capServerPrivate::HandleAcceptCompleteL(TInt aErr)
{
    Q_Q(QL2capServer);

    if (aErr == KErrNone) {
        pendingSocket->setSocketState(QBluetoothSocket::ConnectedState);
        activeSockets.append(pendingSocket);

        pendingSocket = new QBluetoothSocket;
        QBluetoothSocketPrivate *pd = pendingSocket->d_ptr;
        pd->iSocket->Accept(*pd->iSocket);

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

void QL2capServerPrivate::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification &aEventNotification)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QL2capServerPrivate::HandleConnectCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QL2capServerPrivate::HandleIoctlCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QL2capServerPrivate::HandleReceiveCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QL2capServerPrivate::HandleSendCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__ << aErr;
}

void QL2capServerPrivate::HandleShutdownCompleteL(TInt aErr)
{
    if (aErr == KErrNone)
        socket->setSocketState(QBluetoothSocket::UnconnectedState);
    else
        qDebug() << __PRETTY_FUNCTION__ << aErr;
}

QTM_END_NAMESPACE
