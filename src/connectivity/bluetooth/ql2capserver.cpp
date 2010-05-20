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

QTM_BEGIN_NAMESPACE

/*!
    \class QL2capServer
    \brief The QL2capServer class provides an L2CAP server.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    QL2capServer is used to implement Bluetooth services over L2CAP.

    Start listening for incoming connections with listen(). The newConnection() signal is emitted
    when a new connection is established. Call nextPendingConnection() to get a QBluetoothSocket
    for the new connection.

    to enable other devices to find your service create a QBluetoothServiceInfo with the applicable
    attributes for your service and register it with QBluetoothServiceInfo::registerService(). Call
    serverPort() to get the L2CAP port number that is being used.

    \sa QBluetoothServiceInfo, QBluetoothSocket
*/

/*!
    \fn void QL2capServer::newConnection()

    This signal is emitted when a new connection is available.

    The connected slot should call nextPendingConnection() to get a QBluetoothSocket object to send
    and receive data over the connection.

    \sa nextPendingConnection(), hasPendingConnections()
*/

/*!
    \fn void QL2capServer::close()

    Closes and resets the listening socket.
*/

/*!
    \fn bool QL2capServer::listen(const QBluetoothAddress &address, quint16 port)

    Start listening for incoming connections to \a address on \a port.

    Returns true if the operation succeeded and the L2CAP server is listening for incoming
    connections; otherwise returns false.

    \sa isListening(), newConnection()
*/

/*!
    \fn void QL2capServer::setMaxPendingConnections(int numConnections)

    Sets the maximum number of pending connections to \a numConnections.

    \sa maxPendingConnections()
*/

/*!
    \fn bool QL2capServer::hasPendingConnections() const

    Returns true if a connection is pending; otherwise returns false.
*/

/*!
    \fn QBluetoothSocket *QL2capServer::nextPendingConnection()

    Returns a pointer to a QBluetoothSocket for the next pending connection. It is the callers
    responsibility to delete the pointer.
*/

/*!
    \fn QBluetoothAddress QL2capServer::serverAddress() const

    Returns the server address.
*/

/*!
    \fn quint16 QL2capServer::serverPort() const

    Returns the server's port number.
*/

/*!
    Constructs an L2CAP server with \a parent.
*/
QL2capServer::QL2capServer(QObject *parent)
:   QObject(parent)
{
}

/*!
    Destorys the L2CAP server.
*/
QL2capServer::~QL2capServer()
{
}

/*!
    Returns true if the L2CAP server is listening for incoming connections; otherwise returns
    false.
*/
bool QL2capServer::isListening() const
{
}

/*!
    Returns the maximum number of pending connections.

    \sa setMaxPendingConnections()
*/
int QL2capServer::maxPendingConnections() const
{
}

/*!
    Sets the Bluetooth security flags to \a security. This function must be called prior to calling
    listen().
*/
void QL2capServer::setSecurityFlags(QBluetooth::SecurityFlags security)
{
}

/*!
    Returns the Bluetooth security flags.
*/
QBluetooth::SecurityFlags QL2capServer::securityFlags() const
{
    return QBluetooth::NoSecurity;
}

#include "moc_ql2capserver.cpp"

QTM_END_NAMESPACE
