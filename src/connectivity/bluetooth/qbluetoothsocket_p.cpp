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

#include <QtCore/QSocketNotifier>

QTM_BEGIN_NAMESPACE

QBluetoothSocketPrivate::QBluetoothSocketPrivate(QBluetoothSocket *parent)
:socketType(QBluetoothSocket::UnknownSocketType), state(QBluetoothSocket::UnconnectedState),
 socketError(QBluetoothSocket::UnknownSocketError), rxOffset(0), socket(-1),
 readNotifier(0), q(parent)
{
    connect(this, SIGNAL(readyRead()), q, SIGNAL(readyRead()));
    connect(this, SIGNAL(connected()), q, SIGNAL(connected()));
    connect(this, SIGNAL(disconnected()), q, SIGNAL(disconnected()));
    connect(this, SIGNAL(error(QBluetoothSocket::SocketError)), q, SIGNAL(error(QBluetoothSocket::SocketError)));
    connect(this, SIGNAL(stateChanged(QBluetoothSocket::SocketState)), q, SIGNAL(stateChanged(QBluetoothSocket::SocketState)));
}

QBluetoothSocketPrivate::~QBluetoothSocketPrivate()
{
    delete readNotifier;
}

bool QBluetoothSocketPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    qDebug() << "ensureNativeSocket: NOT IMPLEMENTED";
    return false;
}

void QBluetoothSocketPrivate::_q_readNotify()
{
    char *writePointer = buffer.reserve(QBLUETOOTHDEVICE_BUFFERSIZE);
    qint64 readFromDevice = q->readData(writePointer, QBLUETOOTHDEVICE_BUFFERSIZE);
    buffer.chop(QBLUETOOTHDEVICE_BUFFERSIZE - (readFromDevice < 0 ? 0 : int(readFromDevice)));

    emit readyRead();
}

void QBluetoothSocketPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    qDebug() << "connectToService: NOT IMPLEMENTED";
}

#include "moc_qbluetoothsocket_p.cpp"

QTM_END_NAMESPACE
