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

#include "qbluetoothsocket_p.h"
#include "qbluetoothsocket_symbian_p.h"
#include "qbluetoothsocket.h"
#include "utils_symbian_p.h"

#include <QCoreApplication>

#include <QDebug>

#include <limits.h>
#include <bt_sock.h>
#include <es_sock.h>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSocketServerPrivate, getSocketServer)

QBluetoothSocketSymbianPrivate::QBluetoothSocketSymbianPrivate(QBluetoothSocket *parent)
:   QBluetoothSocketPrivate(parent),
    socket(0),
    rxDescriptor(0, 0)    
{
}

QBluetoothSocketSymbianPrivate::~QBluetoothSocketSymbianPrivate()
{
    delete socket;
}

void QBluetoothSocketSymbianPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    TBTSockAddr a;

    a.SetBTAddr(TBTDevAddr(address.toUInt64()));
    a.SetPort(port);

    if (socket->Connect(a) == KErrNone) {
        state = QBluetoothSocket::ConnectingState;
        emit q->stateChanged(state);
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

bool QBluetoothSocketSymbianPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    if (socket) {
        if (socketType == type)
            return true;
        else
            delete socket;
    }

    socketType = type;

    switch (type) {
    case QBluetoothSocket::L2capSocket:
        socket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer, _L("L2CAP"));
        break;
    case QBluetoothSocket::RfcommSocket:
        socket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer, _L("RFCOMM"));
        break;
    default:
        socket = 0;
        return false;
    }

    return true;
}

void QBluetoothSocketSymbianPrivate::ensureBlankNativeSocket()
{
    if (socket)
        delete socket;

    socket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer);
}

void QBluetoothSocketSymbianPrivate::startReceive()
{    
    rxDescriptor.Set(reinterpret_cast<unsigned char *>(buffer.reserve(4096)), 0,  4096);
    if (socket->RecvOneOrMore(rxDescriptor, 0, rxLength) != KErrNone) {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketSymbianPrivate::HandleAcceptCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketSymbianPrivate::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification &aEventNotification)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketSymbianPrivate::HandleConnectCompleteL(TInt aErr)
{
    if (aErr == KErrNone) {
        state = QBluetoothSocket::ConnectedState;
        emit q->stateChanged(state);

        emit q->connected();

        startReceive();
    } else {
        state = QBluetoothSocket::UnconnectedState;
        emit q->stateChanged(state);

        switch (aErr) {
        case KErrCouldNotConnect:
            socketError = QBluetoothSocket::ConnectionRefusedError;
            break;
        default:
            qDebug() << __PRETTY_FUNCTION__ << aErr;
            socketError = QBluetoothSocket::UnknownSocketError;
        }
        emit q->error(socketError);
    }
}

void QBluetoothSocketSymbianPrivate::HandleIoctlCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketSymbianPrivate::HandleReceiveCompleteL(TInt aErr)
{
    if (aErr == KErrNone) {
        if (rxLength() == 0) {
            emit q->readChannelFinished();
            emit q->disconnected();
            return;
        }

        buffer.chop(rxLength());
        
        emit q->readyRead();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketSymbianPrivate::HandleSendCompleteL(TInt aErr)
{
    if (aErr == KErrNone) {
        qint64 writeSize = txBuffer.length();
        txBuffer.clear();
        emit q->bytesWritten(writeSize);

        if (state == QBluetoothSocket::ClosingState)
            q->close();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketSymbianPrivate::HandleShutdownCompleteL(TInt aErr)
{
    if (aErr == KErrNone) {
        state = QBluetoothSocket::UnconnectedState;
        emit q->stateChanged(state);
        emit q->disconnected();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

#include "moc_qbluetoothsocket_symbian_p.cpp"

QTM_END_NAMESPACE
