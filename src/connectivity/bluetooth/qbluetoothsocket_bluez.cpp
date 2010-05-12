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

#include "qbluetoothsocket.h"
#include "qbluetoothsocket_p.h"

#include <qplatformdefs.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

#include <errno.h>
#include <unistd.h>

#include <QtCore/QSocketNotifier>

QTM_BEGIN_NAMESPACE

void QBluetoothSocket::abort()
{
    qDebug() << Q_FUNC_INFO << "not implemented";
}

void QBluetoothSocket::disconnectFromService()
{
    qDebug() << Q_FUNC_INFO << "not implemented";
}

QString QBluetoothSocket::localName() const
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    return QString();
}

QBluetoothAddress QBluetoothSocket::localAddress() const
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    return QBluetoothAddress();
}

quint16 QBluetoothSocket::localPort() const
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    return 0;
}

QString QBluetoothSocket::peerName() const
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    return QString();
}

QBluetoothAddress QBluetoothSocket::peerAddress() const
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    return QBluetoothAddress();
}

quint16 QBluetoothSocket::peerPort() const
{
    qDebug() << Q_FUNC_INFO << "not implemented";
    return 0;
}

qint64 QBluetoothSocket::writeData(const char *data, qint64 maxSize)
{
    Q_D(QBluetoothSocket);

    qDebug() << "writing" << QByteArray(data, maxSize);
    if (::write(d->socket, data, maxSize) != maxSize) {
        d->socketError = QBluetoothSocket::UnknownSocketError;
        emit error(d->socketError);
    }

    emit bytesWritten(maxSize);

    return maxSize;
}

qint64 QBluetoothSocket::readData(char *data, qint64 maxSize)
{
    Q_D(QBluetoothSocket);

    qint64 result = ::read(d->socket, data, maxSize);

    qDebug() << "read" << QByteArray(data, result);

    return result;
}

void QBluetoothSocket::close()
{
    qDebug() << Q_FUNC_INFO << "not implemented";
}

QBluetoothSocketPrivate::QBluetoothSocketPrivate()
:   socketType(QBluetoothSocket::UnknownSocketType), state(QBluetoothSocket::UnconnectedState),
    socketError(QBluetoothSocket::UnknownSocketError), rxOffset(0), socket(-1),
    readNotifier(0)
{
}

QBluetoothSocketPrivate::~QBluetoothSocketPrivate()
{
    delete readNotifier;
}

bool QBluetoothSocketPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    if (socket != -1) {
        if (socketType == type)
            return true;

        delete readNotifier;
        readNotifier = 0;
        QT_CLOSE(socket);
    }

    socketType = type;

    switch (type) {
    case QBluetoothSocket::L2capSocket:
        socket = ::socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        break;
    case QBluetoothSocket::RfcommSocket:
        socket = ::socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        break;
    default:
        socket = -1;
    }

    if (socket == -1)
        return false;

    int flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);

    Q_Q(QBluetoothSocket);

    readNotifier = new QSocketNotifier(socket, QSocketNotifier::Read);
    QObject::connect(readNotifier, SIGNAL(activated(int)), q, SLOT(_q_readNotify()));

    return true;
}

void QBluetoothSocketPrivate::_q_readNotify()
{
    Q_Q(QBluetoothSocket);

    emit q->readyRead();
}

void QBluetoothSocketPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    Q_Q(QBluetoothSocket);

    int result = -1;

    if (socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;

        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = port;

        quint64 bdaddr = address.toUInt64();

        addr.rc_bdaddr.b[0] = (bdaddr >> 0) & 0xff;
        addr.rc_bdaddr.b[1] = (bdaddr >> 8) & 0xff;
        addr.rc_bdaddr.b[2] = (bdaddr >> 16) & 0xff;
        addr.rc_bdaddr.b[3] = (bdaddr >> 24) & 0xff;
        addr.rc_bdaddr.b[4] = (bdaddr >> 32) & 0xff;
        addr.rc_bdaddr.b[5] = (bdaddr >> 40) & 0xff;

        result = connect(socket, (sockaddr *)&addr, sizeof(addr));
    } else if (socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;

        addr.l2_family = AF_BLUETOOTH;
        addr.l2_psm = port;

        quint64 bdaddr = address.toUInt64();

        addr.l2_bdaddr.b[0] = (bdaddr >> 0) & 0xff;
        addr.l2_bdaddr.b[1] = (bdaddr >> 8) & 0xff;
        addr.l2_bdaddr.b[2] = (bdaddr >> 16) & 0xff;
        addr.l2_bdaddr.b[3] = (bdaddr >> 24) & 0xff;
        addr.l2_bdaddr.b[4] = (bdaddr >> 32) & 0xff;
        addr.l2_bdaddr.b[5] = (bdaddr >> 40) & 0xff;

        result = connect(socket, (sockaddr *)&addr, sizeof(addr));
    }

    if (result >= 0) {
        state = QBluetoothSocket::ConnectingState;
        emit q->stateChanged(state);
    } else {
        qDebug() << errno << strerror(errno);
        socketError = QBluetoothSocket::UnknownSocketError;
        emit q->error(socketError);
    }
}

QTM_END_NAMESPACE
