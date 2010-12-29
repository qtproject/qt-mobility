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
#include "qbluetoothservicediscoveryagent.h"
#include "qbluetoothdeviceinfo.h"
#ifdef Q_OS_SYMBIAN
#include "qbluetoothsocket_symbian_p.h"
#else
#include "qbluetoothsocket_bluez_p.h"
#endif

#include <qplatformdefs.h>

#include <errno.h>
#include <unistd.h>

#include <QtCore/QSocketNotifier>

QTM_BEGIN_NAMESPACE

QBluetoothSocketPrivate::QBluetoothSocketPrivate(QBluetoothSocket *parent)
:socketType(QBluetoothSocket::UnknownSocketType), state(QBluetoothSocket::UnconnectedState),
 socketError(QBluetoothSocket::UnknownSocketError),socket(-1),
 readNotifier(0), m_discoveryAgent(0), q(parent), connectWriteNotifier(0)
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
    readNotifier = 0;    
}

bool QBluetoothSocketPrivate::ensureNativeSocket(QBluetoothSocket::SocketType type)
{
    Q_UNUSED(type);
    qDebug() << "ensureNativeSocket: NOT IMPLEMENTED";
    return false;
}

void QBluetoothSocketPrivate::connectToService(const QBluetoothAddress &address, quint16 port, QIODevice::OpenMode openMode)
{
    Q_UNUSED(address);
    Q_UNUSED(port);
    Q_UNUSED(openMode);
    qDebug() << "connectToService: NOT IMPLEMENTED";
}

void QBluetoothSocketPrivate::doDeviceDiscovery(const QBluetoothServiceInfo &service, QBluetoothSocket::OpenMode openMode)
{
    if(m_discoveryAgent) {
        delete m_discoveryAgent;
    }

    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(service.device().address(),this);

    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)), this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));

    m_openMode = openMode;

    if(!service.serviceUuid().isNull())
        m_discoveryAgent->setUuidFilter(service.serviceUuid());

    if(!service.serviceClassUuids().isEmpty())
        m_discoveryAgent->setUuidFilter(service.serviceClassUuids());

    // we have to ID the service somehow
    Q_ASSERT(!m_discoveryAgent->uuidFilter().isEmpty());

    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
}

void QBluetoothSocketPrivate::serviceDiscovered(const QBluetoothServiceInfo &service)
{
    if(service.protocolServiceMultiplexer() != 0 || service.serverChannel() != 0) {
        q->connectToService(service, m_openMode);
        m_discoveryAgent->deleteLater();
        m_discoveryAgent = 0;
    }
}

void QBluetoothSocketPrivate::discoveryFinished()
{
    if(m_discoveryAgent){
        qDebug() << "Could not find service";
        emit error(QBluetoothSocket::UnknownSocketError);
        delete m_discoveryAgent;
    }
}

#include "moc_qbluetoothsocket_p.cpp"

QTM_END_NAMESPACE
