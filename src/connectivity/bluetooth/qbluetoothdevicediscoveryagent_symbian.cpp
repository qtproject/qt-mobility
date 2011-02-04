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
    /* connect to socker server */
    TInt result = m_socketServer.Connect();
    setError(result, QString("RSocketServ.Connect() failed with error"));
}

QBluetoothDeviceDiscoveryAgentPrivate::~QBluetoothDeviceDiscoveryAgentPrivate()
{
    delete m_deviceDiscovery;
    m_socketServer.Close();
}

void QBluetoothDeviceDiscoveryAgentPrivate::start()
{
    // we need to delete previous discovery as only one query could be active a one time
    if (m_deviceDiscovery) {
        delete m_deviceDiscovery;
        m_deviceDiscovery = NULL;
    }
    // clear list of found devices
    discoveredDevices.clear();
    if (m_socketServer.Handle() != NULL) {
        // create new active object for querying devices
        m_deviceDiscovery = new BluetoothLinkManagerDeviceDiscoverer(m_socketServer);
        //bind signals on public interface
        Q_Q(QBluetoothDeviceDiscoveryAgent);
        QObject::connect(m_deviceDiscovery, SIGNAL(deviceDiscoveryComplete(int)), q, SIGNAL(finished()));
        QObject::connect(m_deviceDiscovery, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            q, SLOT(_q_newDeviceFound(const QBluetoothDeviceInfo&)));
        QObject::connect(m_deviceDiscovery, SIGNAL(linkManagerError(QBluetoothDeviceDiscoveryAgent::Error)),
            q, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)));
        // startup the device discovery. Discovery results are obtained from signal connected above.
        if (!m_deviceDiscovery->startDiscovery(inquiryTypeToIAC()))
            setError(KErrNotReady, "Discovery is still active");
    } else {
        setError(KErrBadHandle, QString("RSocketServ.Connect() failed with error"));
    }
}

void QBluetoothDeviceDiscoveryAgentPrivate::stop()
{
    delete m_deviceDiscovery;
    m_deviceDiscovery = NULL;
}

bool QBluetoothDeviceDiscoveryAgentPrivate::isActive() const
{
    if (m_deviceDiscovery)
        return m_deviceDiscovery->IsActive();
    return false;
}

void QBluetoothDeviceDiscoveryAgentPrivate::setError(int errorCode, QString errorDescription)
{
    if (errorCode == KErrNone)
        return;

    errorString = errorDescription;
    if (errorCode == KErrCancel)
        lastError = QBluetoothDeviceDiscoveryAgent::Canceled;
    else
        lastError = QBluetoothDeviceDiscoveryAgent::UnknownError;

    Q_Q(QBluetoothDeviceDiscoveryAgent);
    emit q->error(lastError);
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
