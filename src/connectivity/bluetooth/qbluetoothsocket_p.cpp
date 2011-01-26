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

QTM_BEGIN_NAMESPACE

QBluetoothSocketPrivate::QBluetoothSocketPrivate()
#ifdef Q_OS_SYMBIAN
: rxDescriptor(0)
#endif
{
}

QBluetoothSocketPrivate::~QBluetoothSocketPrivate()
{
}

bool QBluetoothSocketPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    return false;
}

void QBluetoothSocketPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    Q_UNUSED(openMode);
    Q_UNUSED(address);
    Q_UNUSED(port);
}

void QBluetoothSocketPrivate::_q_writeNotify()
{
}

void QBluetoothSocketPrivate::_q_readNotify()
{
}

void QBluetoothSocketPrivate::abort()
{
}

QString QBluetoothSocketPrivate::localName() const
{
    return QString();
}

QBluetoothAddress QBluetoothSocketPrivate::localAddress() const
{
    return QBluetoothAddress();
}

quint16 QBluetoothSocketPrivate::localPort() const
{
    return 0;
}

QString QBluetoothSocketPrivate::peerName() const
{
    return QString();
}

QBluetoothAddress QBluetoothSocketPrivate::peerAddress() const
{
    return QBluetoothAddress();
}

quint16 QBluetoothSocketPrivate::peerPort() const
{
    return 0;
}

qint64 QBluetoothSocketPrivate::writeData(const char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return 0;
}

qint64 QBluetoothSocketPrivate::readData(char *data, qint64 maxSize)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    return 0;
}

void QBluetoothSocketPrivate::close()
{
}

bool QBluetoothSocketPrivate::setSocketDescriptor(int socketDescriptor, QBluetoothSocket::SocketType socketType,
                                           QBluetoothSocket::SocketState socketState, QBluetoothSocket::OpenMode openMode)
{
    Q_UNUSED(socketDescriptor);
    Q_UNUSED(socketType)
    Q_UNUSED(socketState);
    Q_UNUSED(openMode);
    return false;
}

int QBluetoothSocketPrivate::socketDescriptor() const
{
    return 0;
}


QTM_END_NAMESPACE
