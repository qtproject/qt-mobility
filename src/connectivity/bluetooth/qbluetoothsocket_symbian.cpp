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

QString QBluetoothSocket::localName() const
{
    return localAddress().toString();
}

QBluetoothAddress QBluetoothSocket::localAddress() const
{
    TBTSockAddr address;
    
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);

    ds->socket->LocalName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QBluetoothSocket::localPort() const
{
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
    return ds->socket->LocalPort();
}

QString QBluetoothSocket::peerName() const
{
    return peerAddress().toString();
}

QBluetoothAddress QBluetoothSocket::peerAddress() const
{
    TBTSockAddr address;
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
    
    ds->socket->RemoteName(address);

    return qTBTDevAddrToQBluetoothAddress(address.BTAddr());
}

quint16 QBluetoothSocket::peerPort() const
{
    TBTSockAddr address;
    
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
    
    ds->socket->RemoteName(address);

    return address.Port();
}

void QBluetoothSocket::close()
{
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
    if (ds->state != QBluetoothSocket::ConnectedState && ds->state != QBluetoothSocket::ListeningState)
        return;

    ds->state = QBluetoothSocket::ClosingState;
    emit stateChanged(ds->state);

    ds->socket->Shutdown(RSocket::ENormal);
}

void QBluetoothSocket::abort()
{
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
  
    ds->socket->CancelWrite();

    ds->socket->Shutdown(RSocket::EImmediate);

    // force active object to run and shutdown socket.
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
}

qint64 QBluetoothSocket::readData(char *data, qint64 maxSize)
{
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
  
    if (ds->rxLength() == 0)
        return 0;
    
    qint64 size = ds->buffer.read(data, maxSize);

    //d->startReceive();

    return size;
}

qint64 QBluetoothSocket::writeData(const char *data, qint64 maxSize)
{
    QBluetoothSocketSymbianPrivate *ds = qobject_cast<QBluetoothSocketSymbianPrivate *>(d);
  
    if (!ds->txBuffer.isEmpty())
        return 0;

    ds->txBuffer = QByteArray(data, maxSize);

    if (ds->socket->Send(TPtrC8(reinterpret_cast<const unsigned char *>(ds->txBuffer.constData()), ds->txBuffer.length()), 0) != KErrNone) {
        ds->socketError = QBluetoothSocket::UnknownSocketError;
        emit error(ds->socketError);
    }

    return maxSize;
}

void QBluetoothSocketPrivate::_q_readNotify()
{
}

QBluetoothSocketPrivate *QBluetoothSocketPrivate::constructPrivate(QBluetoothSocket *parent)
{
    return new QBluetoothSocketSymbianPrivate(parent);
}


QTM_END_NAMESPACE
