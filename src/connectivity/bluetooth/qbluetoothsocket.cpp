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

#include "qbluetoothdeviceinfo.h"
#include "qbluetoothserviceinfo.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothSocket
    \brief The QBluetoothSocket class provides a Bluetooth socket.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    QBluetoothSocket supports two socket types, \l {QBluetoothSocket::L2CapSocket}{L2CAP} and
    \l {QBluetoothSocket::RfcommSocket}{RFCOMM}.

    \l {QBluetoothSocket::L2CapSocket}{L2CAP} is a low level datagram-oriented Bluetooth socket.

    \l {QBluetoothSocket::RfcommSocket}{RFCOMM} is a reliable, stream-oriented socket.  RFCOMM
    sockets emulate an RS-232 serial port.

    To create a connection to a Bluetooth service create a socket of the appropriate type and call
    connectToService() passing the Bluetooth address and port number. QBluetoothSocket will emit
    the connected() signal when the connection is established.


*/

/*!
    \enum QBluetoothSocket::SocketType

    This enum describes the Bluetooth socket type.

    \value UnknownSocketType    Unknown socket type.
    \value L2CapSocket          L2CAP socket.
    \value RfcommSocket         RFCOMM socket.
*/

/*!
    \enum QBluetoothSocket::SocketState

    This enum describes the state of the Bluetooth socket.

    \value UnconnectedState      Socket is not connected.
    \value ServiceLookupState   Socket is querying connection parameters.
    \value ConnectingState      Socket is attempting to connect to a device.
    \value ConnectedState       Socket is connected to a device.
    \value BoundState           Socket is bound to a local address and port.
    \value ClosingState         Socket is connected and will be closed once all pending data is
                                written to the socket.
    \value ListeningState       Socket is listening for incoming connections.
*/

/*!
    \enum QBluetoothSocket::SocketError

    This enum describes Bluetooth socket error types.

    \value UnknownSocketError       An unknown error has occurred.
    \value ConnectionRefusedError   Connection refused or device not available.
*/

/*!
    \fn void QBluetoothSocket::connected()

    This signal is emitted when a connection is established.

    \sa QBluetoothSocket::ConnectedState, stateChanged()
*/

/*!
    \fn void QBluetoothSocket::disconnected()

    This signal is emitted when the socket is disconnected.

    \sa QBluetoothSocket::UnconnectedState, stateChanged()
*/

/*!
    \fn void QBluetoothSocket::error(QBluetoothSocket::SocketError error)

    This signal is emitted when an \a error occurs.

    \sa error()
*/

/*!
    \fn QBluetoothSocket::stateChanged(QBluetoothSocket::SocketState state)

    This signal is emitted when the socket state changes to \a state.

    \sa connected(), disconnected(), state(), QBluetoothSocket::SocketState
*/

/*!
    \fn void QBluetoothSocket::abort()

    Aborts the current connection and resets the socket. Unlike disconnectFromService(), this
    function immediately closes the socket, discarding any pending data in the write buffer.

    \sa disconnectFromService(), close()
*/

/*!
    \fn void QBluetoothSocket::close()

    Disconnects the socket's connection with the device.
*/

/*!
    \fn void QBluetoothSocket::disconnectFromService()

    Attempts to close the socket. If there is pending data waiting to be written QBluetoothSocket
    will enter ClosingState and wait until all data has been written. Eventually, it will enter
    UnconnectedState and emit the disconnected() signal.

    \sa connectToService()
*/

/*!
    \fn QString QBluetoothSocket::localName() const

    Returns the name of the local device.
*/

/*!
    \fn QBluetoothAddress QBluetoothSocket::localAddress() const

    Returns the address of the local device.
*/

/*!
    \fn quint16 QBluetoothSocket::localPort() const

    Returns the port number of the local socket if available; otherwise returns 0.
*/

/*!
    \fn QString QBluetoothSocket::peerName() const

    Returns the name of the peer device.
*/

/*!
    \fn QBluetoothAddress QBluetoothSocket::peerAddress() const

    Returns the address of the peer device.
*/

/*!
    \fn quint16 QBluetoothSocket::peerPort() const

    Return the port number of the peer socket if available; otherwise returns 0.
*/

/*!
    \fn qint64 QBluetoothSocket::readData(char *data, qint64 maxSize)

    \reimp
*/

/*!
    \fn qint64 QBluetoothSocket::writeData(const char *data, qint64 maxSize)

    \reimp
*/

/*!
    Constructs a Bluetooth socket of \a socketType type, with \a parent.
*/
QBluetoothSocket::QBluetoothSocket(QBluetoothSocket::SocketType socketType, QObject *parent)
: QIODevice(parent), d_ptr(new QBluetoothSocketPrivate)
{
    Q_D(QBluetoothSocket);

    d->q_ptr = this;
    d->ensureNativeSocket(socketType);

    setOpenMode(QIODevice::ReadWrite);
}

/*!
    Constructs a Bluetooth socket with \a parent.
*/
QBluetoothSocket::QBluetoothSocket(QObject *parent)
: QIODevice(parent), d_ptr(new QBluetoothSocketPrivate)
{
    d_ptr->q_ptr = this;

    setOpenMode(QIODevice::ReadWrite);
}

/*!
    Destroys the Bluetooth socket.
*/
QBluetoothSocket::~QBluetoothSocket()
{
    delete d_ptr;
}

/*!
    \reimp
*/
bool QBluetoothSocket::isSequential() const
{
    return true;
}

/*!
    Returns the number of incoming bytes that are waiting to be read.

    \sa bytesToWrite(), read()
*/
qint64 QBluetoothSocket::bytesAvailable() const
{
    Q_D(const QBluetoothSocket);

    if (d->rxOffset == -1)
        return QIODevice::bytesAvailable();
    else
        return QIODevice::bytesAvailable() + d->rxBuffer.length() - d->rxOffset;
}

/*!
    Returns the number of bytes that are waiting to be written. The bytes are written when control
    goes back to the event loop.
*/
qint64 QBluetoothSocket::bytesToWrite() const
{
    Q_D(const QBluetoothSocket);

    return d->txBuffer.length();
}

/*!
    Returns true if a line of data can be read from the socket; otherwise returns false.

    \sa readLine()
*/
bool QBluetoothSocket::canReadLine() const
{
    Q_D(const QBluetoothSocket);

    return (d->rxBuffer.indexOf('\n', d->rxOffset) != -1) || QIODevice::canReadLine();
}

/*!
    Attempts to connect to the service described by \a service.

    The socket is opened in the given \a openMode.

    The socket first enters ConnectingState and attempts to connect to the device providing
    \a service. If a connection is established, QBluetoothSocket enters ConnectedState and
    emits connected().

    At any point, the socket can emit error() to siganl that an error occurred.

    \sa state(), disconnectFromService()
*/
void QBluetoothSocket::connectToService(const QBluetoothServiceInfo &service, OpenMode openMode)
{
    Q_D(QBluetoothSocket);

    setOpenMode(openMode);

    if (service.protocolServiceMultiplexer() > 0) {
        if (!d->ensureNativeSocket(L2CapSocket)) {
            emit error(UnknownSocketError);
            return;
        }
        d->connectToService(service.device().address(), service.protocolServiceMultiplexer(), openMode);
    } else if (service.serverChannel() > 0) {
        if (!d->ensureNativeSocket(RfcommSocket)) {
            emit error(UnknownSocketError);
            return;
        }
        d->connectToService(service.device().address(), service.serverChannel(), openMode);
    } else {
        emit error(UnknownSocketError);
    }
}

/*!
    Attempts to make a connection to the service identified by \a uuid on the device with address
    \a address.

    The socket is opened in the given \a openMode.

    The socket first enters the ServiceLookupState and queries the connection parameters for
    \a uuid. If the service parameters are successfully retrieved the socket enters
    ConnectingState, and attempts to connect to \a address. If a connection is established,
    QBluetoothSocket enters Connected State and emits connected().

    At any point, the socket can emit error() to signal that an error occurred.

    \sa state(), disconnectFromService()
*/
void QBluetoothSocket::connectToService(const QBluetoothAddress &address, const QBluetoothUuid &uuid, OpenMode openMode)
{
}

/*!
    Attempts to make a connection with \a address on the given \a port.

    The socket is opened in the given \a openMode.

    The socket first enters ConnectingState, and attempts to connect to \a address. If a
    connection is established, QBluetoothSocket enters ConnectedState and emits connected().

    At any point, the socket can emit error() to signal that an error occurred.

    \sa state(), disconnectFromService()
*/
void QBluetoothSocket::connectToService(const QBluetoothAddress &address, quint16 port, OpenMode openMode)
{
    Q_D(QBluetoothSocket);

    setOpenMode(openMode);

    d->connectToService(address, port, openMode);
}

/*!
    Returns the socket type.
*/
QBluetoothSocket::SocketType QBluetoothSocket::socketType() const
{
    Q_D(const QBluetoothSocket);

    return d->socketType;
}

/*!
    Returns the current state of the socket.
*/
QBluetoothSocket::SocketState QBluetoothSocket::state() const
{
    Q_D(const QBluetoothSocket);

    return d->state;
}

/*!
    Returns the last error.
*/
QBluetoothSocket::SocketError QBluetoothSocket::error() const
{
    Q_D(const QBluetoothSocket);

    return d->socketError;
}

/*!
    Sets the socket state to \a state.
*/
void QBluetoothSocket::setSocketState(QBluetoothSocket::SocketState state)
{
    Q_D(QBluetoothSocket);

    d->state = state;
}

/*!
    Sets the type of error that last occurred to \a error.
*/
void QBluetoothSocket::setSocketError(QBluetoothSocket::SocketError error)
{
    Q_D(QBluetoothSocket);

    d->socketError = error;
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, QBluetoothSocket::SocketError error)
{
    switch (error) {
    case QBluetoothSocket::UnknownSocketError:
        debug << "QBluetoothSocket::UnknownSocketError";
        break;
    default:
        debug << "QBluetoothSocket::SocketError(" << error << ")";
    }
    return debug;
}

QDebug operator<<(QDebug debug, QBluetoothSocket::SocketState state)
{
    switch (state) {
    case QBluetoothSocket::UnconnectedState:
        debug << "QBluetoothSocket::UnconnectedState";
        break;
    case QBluetoothSocket::ConnectingState:
        debug << "QBluetoothSocket::ConnectingState";
        break;
    case QBluetoothSocket::ConnectedState:
        debug << "QBluetoothSocket::ConnectedState";
        break;
    case QBluetoothSocket::BoundState:
        debug << "QBluetoothSocket::BoundState";
        break;
    case QBluetoothSocket::ClosingState:
        debug << "QBluetoothSocket::ClosingState";
        break;
    case QBluetoothSocket::ListeningState:
        debug << "QBluetoothSocket::ListeningState";
        break;
    default:
        debug << "QBluetoothSocket::SocketState(" << state << ")";
    }
    return debug;
}
#endif

#include "moc_qbluetoothsocket.cpp"

QTM_END_NAMESPACE
