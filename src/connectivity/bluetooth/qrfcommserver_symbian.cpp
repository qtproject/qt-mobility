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
#include "qbluetoothsocket_p.h"
#include "utils_symbian_p.h"

#include <QTimer>
#include <QCoreApplication>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QRfcommServerPrivate::QRfcommServerPrivate()
: maxPendingConnections(1), pendingSocket(0)
{
    socket = new QBluetoothSocket(QBluetoothSocket::RfcommSocket);
    socket->d_func()->socket->SetNotifier(*this);
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

    if (address.isValid())
        addr.SetBTAddr(TBTDevAddr(address.toUInt64()));
    if (port == 0)
        addr.SetPort(KRfcommPassiveAutoBind);
    else
        addr.SetPort(port);

    TBTServiceSecurity security;
    addr.SetSecurity(security);
    d->socket->d_func()->socket->Bind(addr);
    d->socket->setSocketState(QBluetoothSocket::BoundState);

    d->socket->d_func()->socket->Listen(d->maxPendingConnections);

    d->pendingSocket = new QBluetoothSocket;
    d->pendingSocket->d_func()->ensureBlankNativeSocket();
    if (d->socket->d_func()->socket->Accept(*d->pendingSocket->d_func()->socket) == KErrNone)
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

    TBTSockAddr address;
    d->socket->d_func()->socket->LocalName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QRfcommServer::serverPort() const
{
    Q_D(const QRfcommServer);

    if (d->socket->state() == QBluetoothSocket::UnconnectedState)
        return 0;

    return d->socket->d_func()->socket->LocalPort();
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

    next->d_func()->startReceive();

    return next;
}

void QRfcommServerPrivate::HandleAcceptCompleteL(TInt aErr)
{
    Q_Q(QRfcommServer);

    if (aErr == KErrNone) {
        pendingSocket->setSocketState(QBluetoothSocket::ConnectedState);
        activeSockets.append(pendingSocket);

        pendingSocket = new QBluetoothSocket;
        socket->d_func()->socket->Accept(*pendingSocket->d_func()->socket);

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
