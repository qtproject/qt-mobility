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
#include "qbluetoothsocket.h"
#include "utils_symbian_p.h"

#include <QCoreApplication>

#include <QDebug>

#include <limits.h>
#include <bt_sock.h>
#include <es_sock.h>

QTM_BEGIN_NAMESPACE

class QSocketServerPrivate
{
public:
    QSocketServerPrivate();
    ~QSocketServerPrivate();

    RSocketServ socketServer;
};

Q_GLOBAL_STATIC(QSocketServerPrivate, getSocketServer)

QSocketServerPrivate::QSocketServerPrivate()
{
    /* connect to socket server */
    TInt result = socketServer.Connect();
    if (result != KErrNone) {
        qWarning("%s: RSocketServ.Connect() failed with error %d", __PRETTY_FUNCTION__, result);
        return;
    }
}

QSocketServerPrivate::~QSocketServerPrivate()
{
    if (socketServer.Handle() != 0)
        socketServer.Close();
}

QBluetoothSocketPrivate::QBluetoothSocketPrivate()
:   rxOffset(0), rxDescriptor(0, 0),
    socketType(QBluetoothSocket::UnknownSocketType),
    state(QBluetoothSocket::UnconnectedState),
    socketError(QBluetoothSocket::UnknownSocketError),
    socket(0)
{
    //socket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer);
}

QBluetoothSocketPrivate::QBluetoothSocketPrivate(QBluetoothSocket::SocketType socketType)
:   rxOffset(0), rxDescriptor(0, 0),
    socketType(socketType),
    state(QBluetoothSocket::UnconnectedState),
    socketError(QBluetoothSocket::UnknownSocketError),
    socket(0)
{
    ensureNativeSocket(socketType);
}

QBluetoothSocketPrivate::~QBluetoothSocketPrivate()
{
    delete socket;
}

void QBluetoothSocketPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    Q_Q(QBluetoothSocket);

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

bool QBluetoothSocketPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    if (socket) {
        if (socketType == type)
            return true;
        else
            delete socket;
    }

    socketType = type;

    switch (type) {
    case QBluetoothSocket::L2CapSocket:
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

void QBluetoothSocketPrivate::ensureBlankNativeSocket()
{
    if (socket)
        delete socket;

    socket = CBluetoothSocket::NewL(*this, getSocketServer()->socketServer);
}

void QBluetoothSocket::disconnectFromService()
{
    Q_D(QBluetoothSocket);

    if (d->state != QBluetoothSocket::ConnectedState)
        return;

    d->state = QBluetoothSocket::ClosingState;
    emit stateChanged(d->state);

    d->socket->Shutdown(RSocket::EStopInput);
}

QString QBluetoothSocket::localName() const
{
    return localAddress().toString();
}

QBluetoothAddress QBluetoothSocket::localAddress() const
{
    Q_D(const QBluetoothSocket);

    TBTSockAddr address;

    d->socket->LocalName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QBluetoothSocket::localPort() const
{
    Q_D(const QBluetoothSocket);

    return d->socket->LocalPort();
}

QString QBluetoothSocket::peerName() const
{
    return peerAddress().toString();
}

QBluetoothAddress QBluetoothSocket::peerAddress() const
{
    Q_D(const QBluetoothSocket);

    TBTSockAddr address;

    d->socket->RemoteName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QBluetoothSocket::peerPort() const
{
    Q_D(const QBluetoothSocket);

    TBTSockAddr address;

    d->socket->RemoteName(address);

    return address.Port();
}

void QBluetoothSocket::close()
{
    Q_D(QBluetoothSocket);

    if (d->state != QBluetoothSocket::ConnectedState && d->state != QBluetoothSocket::ListeningState)
        return;

    d->state = QBluetoothSocket::ClosingState;
    emit stateChanged(d->state);

    d->socket->Shutdown(RSocket::ENormal);
}

void QBluetoothSocket::abort()
{
    Q_D(QBluetoothSocket);

    d->socket->CancelWrite();

    d->socket->Shutdown(RSocket::EImmediate);

    // force active object to run and shutdown socket.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

qint64 QBluetoothSocket::readData(char *data, qint64 maxSize)
{
    Q_D(QBluetoothSocket);

    if (d->rxLength() == 0 || d->rxOffset == -1)
        return 0;

    qint64 readSize = qMin(maxSize, d->rxBuffer.length() - d->rxOffset);

    memcpy(data, d->rxBuffer.constData() + d->rxOffset, readSize);

    d->rxOffset += readSize;
    if (d->rxOffset >= d->rxBuffer.length())
        d->startReceive();

    return readSize;
}

qint64 QBluetoothSocket::writeData(const char *data, qint64 maxSize)
{
    Q_D(QBluetoothSocket);

    if (!d->txBuffer.isEmpty())
        return 0;

    d->txBuffer = QByteArray(data, maxSize);

    if (d->socket->Send(TPtrC8(reinterpret_cast<const unsigned char *>(d->txBuffer.constData()), d->txBuffer.length()), 0) != KErrNone) {
        d->socketError = QBluetoothSocket::UnknownSocketError;
        emit error(d->socketError);
    }

    return maxSize;
}

void QBluetoothSocketPrivate::startReceive()
{
    Q_Q(QBluetoothSocket);

    rxOffset = -1;
    rxBuffer.resize(4096);
    rxDescriptor.Set(reinterpret_cast<unsigned char *>(rxBuffer.data()), 0, rxBuffer.length());
    if (socket->RecvOneOrMore(rxDescriptor, 0, rxLength) != KErrNone) {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketPrivate::HandleAcceptCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketPrivate::HandleActivateBasebandEventNotifierCompleteL(TInt aErr, TBTBasebandEventNotification &aEventNotification)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketPrivate::HandleConnectCompleteL(TInt aErr)
{
    Q_Q(QBluetoothSocket);

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

void QBluetoothSocketPrivate::HandleIoctlCompleteL(TInt aErr)
{
    qDebug() << __PRETTY_FUNCTION__;
}

void QBluetoothSocketPrivate::HandleReceiveCompleteL(TInt aErr)
{
    Q_Q(QBluetoothSocket);

    if (aErr == KErrNone) {
        if (rxLength() == 0) {
            emit q->readChannelFinished();
            emit q->disconnected();
            return;
        }

        rxBuffer.resize(rxLength());

        rxOffset = 0;

        emit q->readyRead();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

void QBluetoothSocketPrivate::HandleSendCompleteL(TInt aErr)
{
    Q_Q(QBluetoothSocket);

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

void QBluetoothSocketPrivate::HandleShutdownCompleteL(TInt aErr)
{
    Q_Q(QBluetoothSocket);

    if (aErr == KErrNone) {
        state = QBluetoothSocket::UnconnectedState;
        emit q->stateChanged(state);
        emit q->disconnected();
    } else {
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

QTM_END_NAMESPACE
