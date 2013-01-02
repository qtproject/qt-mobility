/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbluetoothdevicediscoveryagent_p.h"
#include "qbluetoothaddress.h"

#include <bttypes.h>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QBluetoothDeviceDiscoveryAgentPrivate::QBluetoothDeviceDiscoveryAgentPrivate()
    : inquiryType(QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry)
    , lastError(QBluetoothDeviceDiscoveryAgent::NoError)
    , errorString(QString())
    , m_deviceDiscovery(0)
{
}

QBluetoothDeviceDiscoveryAgentPrivate::~QBluetoothDeviceDiscoveryAgentPrivate()
{
    delete m_deviceDiscovery;
    if (m_socketServer.Handle() != NULL) 
        m_socketServer.Close();
}

void QBluetoothDeviceDiscoveryAgentPrivate::allocate()
{
    Q_Q(QBluetoothDeviceDiscoveryAgent);
    /* connect to socker server */
    int result = m_socketServer.Connect();
    if (result != KErrNone) {
        _q_setError(QBluetoothDeviceDiscoveryAgent::UnknownError, 
                QString("RSocketServ.Connect() failed with error"));
        return;
    }
    // create link manager device discoverer
    if (m_socketServer.Handle() != NULL) {
        // create new active object for querying devices
        m_deviceDiscovery = new BluetoothLinkManagerDeviceDiscoverer(m_socketServer);
        if (m_deviceDiscovery) {
            QObject::connect(m_deviceDiscovery, SIGNAL(deviceDiscoveryComplete()), q, SIGNAL(finished()));
            QObject::connect(m_deviceDiscovery, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
                q, SLOT(_q_newDeviceFound(const QBluetoothDeviceInfo&)));
            QObject::connect(m_deviceDiscovery, SIGNAL(linkManagerError(QBluetoothDeviceDiscoveryAgent::Error,QString)),
                q, SLOT(_q_setError(QBluetoothDeviceDiscoveryAgent::Error,QString)));
            QObject::connect(m_deviceDiscovery, SIGNAL(canceled()), q, SIGNAL(canceled()));
        } else {
            _q_setError(QBluetoothDeviceDiscoveryAgent::UnknownError, 
                    QString("Cannot allocate BluetoothLinkManagerDeviceDiscoverer: failed with error"));
        }
    } 
}

void QBluetoothDeviceDiscoveryAgentPrivate::start()
{
    // clear list of found devices
    discoveredDevices.clear();
    if (!m_deviceDiscovery) {
        allocate();
    } 
    if(m_deviceDiscovery) {
        m_deviceDiscovery->startDiscovery(inquiryTypeToIAC());
    }

}

void QBluetoothDeviceDiscoveryAgentPrivate::stop()
{
    if (m_deviceDiscovery)
        m_deviceDiscovery->stopDiscovery();
}

bool QBluetoothDeviceDiscoveryAgentPrivate::isActive() const
{
    if (m_deviceDiscovery)
        return m_deviceDiscovery->isReallyActive();
    return false;
}

void QBluetoothDeviceDiscoveryAgentPrivate::_q_setError(QBluetoothDeviceDiscoveryAgent::Error errorCode, 
        QString errorDescription)
{
    qDebug() << __PRETTY_FUNCTION__ << errorCode << errorDescription;
    if (errorCode == KErrNone)
        return;

    Q_Q(QBluetoothDeviceDiscoveryAgent);

    errorString = errorDescription;
    emit q->error(errorCode);
}

void QBluetoothDeviceDiscoveryAgentPrivate::_q_newDeviceFound(const QBluetoothDeviceInfo &device)
{
    // add found device to the list of devices
    discoveredDevices.append(device);
    Q_Q(QBluetoothDeviceDiscoveryAgent);
    emit q->deviceDiscovered(device);
}

uint QBluetoothDeviceDiscoveryAgentPrivate::inquiryTypeToIAC() const
{
    return inquiryType == QBluetoothDeviceDiscoveryAgent::LimitedInquiry ? KLIAC : KGIAC;
}

QTM_END_NAMESPACE
