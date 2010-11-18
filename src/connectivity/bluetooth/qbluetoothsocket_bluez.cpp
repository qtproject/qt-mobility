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

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"
#include "bluez/device_p.h"

#include <qplatformdefs.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

#include <errno.h>
#include <unistd.h>

#include <QtCore/QSocketNotifier>

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

static inline void convertAddress(quint8 (&from)[6], quint64 &to)
{
    to = (quint64(from[0]) << 0) |
         (quint64(from[1]) << 8) |
         (quint64(from[2]) << 16) |
         (quint64(from[3]) << 24) |
         (quint64(from[4]) << 32) |
         (quint64(from[5]) << 40);
}

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
    Q_D(const QBluetoothSocket);

    if (!d->localName.isEmpty())
        return d->localName;

    const QBluetoothAddress address = localAddress();
    if (address.isNull())
        return QString();

    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> reply = manager.FindAdapter(address.toString());
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    OrgBluezAdapterInterface adapter(QLatin1String("org.bluez"), reply.value().path(),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QVariantMap> properties = adapter.GetProperties();
    properties.waitForFinished();
    if (properties.isError())
        return QString();

    d->localName = properties.value().value(QLatin1String("Name")).toString();

    return d->localName;
}

QBluetoothAddress QBluetoothSocket::localAddress() const
{
    Q_D(const QBluetoothSocket);

    if (d->socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;
        socklen_t addrLength = sizeof(addr);

        if (::getsockname(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0) {
            quint64 bdaddr;
            convertAddress(addr.rc_bdaddr.b, bdaddr);
            return QBluetoothAddress(bdaddr);
        }
    } else if (d->socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;
        socklen_t addrLength = sizeof(addr);

        if (::getsockname(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0) {
            quint64 bdaddr;
            convertAddress(addr.l2_bdaddr.b, bdaddr);
            return QBluetoothAddress(bdaddr);
        }
    }

    return QBluetoothAddress();
}

quint16 QBluetoothSocket::localPort() const
{
    Q_D(const QBluetoothSocket);

    if (d->socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;
        socklen_t addrLength = sizeof(addr);

        if (::getsockname(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0)
            return addr.rc_channel;
    } else if (d->socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;
        socklen_t addrLength = sizeof(addr);

        if (::getsockname(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0)
            return addr.l2_psm;
    }

    return 0;
}

QString QBluetoothSocket::peerName() const
{
    Q_D(const QBluetoothSocket);

    if (!d->peerName.isEmpty())
        return d->peerName;

    quint64 bdaddr;

    if (d->socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;
        socklen_t addrLength = sizeof(addr);

        if (::getpeername(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) < 0)
            return QString();

        convertAddress(addr.rc_bdaddr.b, bdaddr);
    } else if (d->socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;
        socklen_t addrLength = sizeof(addr);

        if (::getpeername(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) < 0)
            return QString();

        convertAddress(addr.l2_bdaddr.b, bdaddr);
    } else {
        return QString();
    }

    const QString address = QBluetoothAddress(bdaddr).toString();

    OrgBluezManagerInterface manager(QLatin1String("org.bluez"), QLatin1String("/"),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> reply = manager.DefaultAdapter();
    reply.waitForFinished();
    if (reply.isError())
        return QString();

    OrgBluezAdapterInterface adapter(QLatin1String("org.bluez"), reply.value().path(),
                                     QDBusConnection::systemBus());

    QDBusPendingReply<QDBusObjectPath> deviceObjectPath = adapter.CreateDevice(address);
    deviceObjectPath.waitForFinished();
    if (deviceObjectPath.isError()) {
        if (deviceObjectPath.error().name() != QLatin1String("org.bluez.Error.AlreadyExists"))
            return QString();

        deviceObjectPath = adapter.FindDevice(address);
        deviceObjectPath.waitForFinished();
        if (deviceObjectPath.isError())
            return QString();
    }

    OrgBluezDeviceInterface device(QLatin1String("org.bluez"), deviceObjectPath.value().path(),
                                   QDBusConnection::systemBus());

    QDBusPendingReply<QVariantMap> properties = device.GetProperties();
    properties.waitForFinished();
    if (properties.isError())
        return QString();

    d->peerName = properties.value().value(QLatin1String("Alias")).toString();

    return d->peerName;
}

QBluetoothAddress QBluetoothSocket::peerAddress() const
{
    Q_D(const QBluetoothSocket);

    if (d->socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;
        socklen_t addrLength = sizeof(addr);

        if (::getpeername(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0) {
            quint64 bdaddr;
            convertAddress(addr.rc_bdaddr.b, bdaddr);
            return QBluetoothAddress(bdaddr);
        }
    } else if (d->socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;
        socklen_t addrLength = sizeof(addr);

        if (::getpeername(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0) {
            quint64 bdaddr;
            convertAddress(addr.l2_bdaddr.b, bdaddr);
            return QBluetoothAddress(bdaddr);
        }
    }

    return QBluetoothAddress();
}

quint16 QBluetoothSocket::peerPort() const
{
    Q_D(const QBluetoothSocket);

    if (d->socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;
        socklen_t addrLength = sizeof(addr);

        if (::getpeername(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0)
            return addr.rc_channel;
    } else if (d->socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;
        socklen_t addrLength = sizeof(addr);

        if (::getpeername(d->socket, reinterpret_cast<sockaddr *>(&addr), &addrLength) == 0)
            return addr.l2_psm;
    }

    return 0;
}

qint64 QBluetoothSocket::writeData(const char *data, qint64 maxSize)
{
    Q_D(QBluetoothSocket);

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

    return ::read(d->socket, data, maxSize);
}

void QBluetoothSocket::close()
{
    Q_D(QBluetoothSocket);

    setSocketState(ClosingState);

    ::close(d->socket);

    setSocketState(UnconnectedState);
}

bool QBluetoothSocket::setSocketDescriptor(int socketDescriptor, SocketType socketType,
                                           SocketState socketState, OpenMode openMode)
{
    Q_D(QBluetoothSocket);

    if (d->readNotifier)
        delete d->readNotifier;

    d->socketType = socketType;
    d->socket = socketDescriptor;

    // ensure that O_NONBLOCK is set on new connections.
    int flags = fcntl(d->socket, F_GETFL, 0);
    if (!(flags & O_NONBLOCK))
        fcntl(d->socket, F_SETFL, flags | O_NONBLOCK);

    d->readNotifier = new QSocketNotifier(d->socket, QSocketNotifier::Read);
    connect(d->readNotifier, SIGNAL(activated(int)), this, SLOT(_q_readNotify()));

    setSocketState(socketState);
    setOpenMode(openMode);

    return true;
}

int QBluetoothSocket::socketDescriptor() const
{
    Q_D(const QBluetoothSocket);

    return d->socket;
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

    char *writePointer = buffer.reserve(QIODEVICE_BUFFERSIZE);
    qint64 readFromDevice = q->readData(writePointer, QIODEVICE_BUFFERSIZE);
    buffer.chop(QIODEVICE_BUFFERSIZE - (readFromDevice < 0 ? 0 : int(readFromDevice)));

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

        convertAddress(address.toUInt64(), addr.rc_bdaddr.b);

        result = connect(socket, (sockaddr *)&addr, sizeof(addr));
    } else if (socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;

        addr.l2_family = AF_BLUETOOTH;
        addr.l2_psm = port;

        convertAddress(address.toUInt64(), addr.l2_bdaddr.b);

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
