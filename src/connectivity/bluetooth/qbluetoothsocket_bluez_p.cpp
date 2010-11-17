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
#include "qbluetoothsocket_bluez_p.h"

#include "bluez/manager_p.h"
#include "bluez/adapter_p.h"
#include "bluez/device_p.h"

#include <qplatformdefs.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/l2cap.h>

#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <QtCore/QSocketNotifier>

QTM_BEGIN_NAMESPACE

QBluetoothSocketBluezPrivate::QBluetoothSocketBluezPrivate(QBluetoothSocket *parent)
    : QBluetoothSocketPrivate(parent), connecting(false)
{
}

QBluetoothSocketBluezPrivate::~QBluetoothSocketBluezPrivate()
{
    delete readNotifier;
    readNotifier = 0;
}

bool QBluetoothSocketBluezPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
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

    readNotifier = new QSocketNotifier(socket, QSocketNotifier::Read);    
    QObject::connect(readNotifier, SIGNAL(activated(int)), this, SLOT(_q_readNotify()));

    return true;
}

void QBluetoothSocketBluezPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    int result = -1;

    if (socketType == QBluetoothSocket::RfcommSocket) {
        sockaddr_rc addr;

        addr.rc_family = AF_BLUETOOTH;
        addr.rc_channel = port;

        convertAddress(address.toUInt64(), addr.rc_bdaddr.b);

        result = ::connect(socket, (sockaddr *)&addr, sizeof(addr));
    } else if (socketType == QBluetoothSocket::L2capSocket) {
        sockaddr_l2 addr;

        addr.l2_family = AF_BLUETOOTH;
        addr.l2_psm = port;

        convertAddress(address.toUInt64(), addr.l2_bdaddr.b);

        result = ::connect(socket, (sockaddr *)&addr, sizeof(addr));
    }

    if (result >= 0 || (result == -1 && errno == EINPROGRESS)) {
        connecting = true;
        q->setSocketState(QBluetoothSocket::ConnectingState);
        connectWriteNotifier = new QSocketNotifier(socket, QSocketNotifier::Write, this);
        connect(connectWriteNotifier, SIGNAL(activated(int)), this, SLOT(writeNotify()));
    } else {
        errorString = QString::fromLocal8Bit(strerror(errno));
        q->setSocketError(QBluetoothSocket::UnknownSocketError);
    }
}

QBluetoothSocketPrivate *QBluetoothSocketPrivate::constructPrivate(QBluetoothSocket *parent)
{
    return new QBluetoothSocketBluezPrivate(parent);
}

void QBluetoothSocketBluezPrivate::writeNotify()
{
    if(connecting && q->state() == QBluetoothSocket::ConnectingState){
        int errorno, len;
        len = sizeof(errorno);
        ::getsockopt(socket, SOL_SOCKET, SO_ERROR, &errorno, (socklen_t*)&len);
        if(errorno) {
            errorString = QString::fromLocal8Bit(strerror(errorno));
            emit error(QBluetoothSocket::UnknownSocketError);
            return;
        }
        // TODO: is this right? set socket state, then emit connected?
        q->setSocketState(QBluetoothSocket::ConnectedState);
        emit connected();

        // TODO: implement write buffering and enable this
        // this CAN NOT BE DELETE.  NON BLOCKING SOCKET
        // means writes silently will fail without this!!!!!!!!
        delete connectWriteNotifier;
        connectWriteNotifier = 0;

    }
    else {
        // error at this time
        delete connectWriteNotifier;
        connectWriteNotifier = 0;
    }
    connecting = false;
}

#include "moc_qbluetoothsocket_bluez_p.cpp"

QTM_END_NAMESPACE
