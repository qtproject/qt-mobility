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

#include "qbluetoothdevicediscoveryagent_symbian_p.h"
#include "qbluetoothaddress.h"

#include <bttypes.h>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QBluetoothDeviceDiscoveryAgentPrivateSymbian::QBluetoothDeviceDiscoveryAgentPrivateSymbian(QObject *parent)
    : QBluetoothDeviceDiscoveryAgentPrivate(parent)
    , m_deviceDiscovery(0)
{
    TInt result;
    /* connect to socker server */
    result = m_socketServer.Connect();
    if (result != KErrNone) {
        setError(result,QString("RSocketServ.Connect() failed with error"));
        return;
    }
}

QBluetoothDeviceDiscoveryAgentPrivateSymbian::~QBluetoothDeviceDiscoveryAgentPrivateSymbian()
{
    disconnect(m_deviceDiscovery);
    delete m_deviceDiscovery;
    m_deviceDiscovery = NULL;
}

void QBluetoothDeviceDiscoveryAgentPrivateSymbian::start()
{
    // we need to delete previous discovery as only one query could be active a one time
    if (m_deviceDiscovery) {
        disconnect(m_deviceDiscovery);
        delete m_deviceDiscovery;
        m_deviceDiscovery = NULL;
    }
    // clear list of found devices
    discoveredDevices.clear();
    // create new active object for querying devices
    m_deviceDiscovery = new BluetoothLinkManagerDeviceDiscoverer(m_socketServer);
    //TODO fix signal
    connect(m_deviceDiscovery, SIGNAL(deviceDiscoveryComplete(int)), this, SIGNAL(finished()));
    connect(m_deviceDiscovery, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
        this, SLOT(newDeviceFound(const QBluetoothDeviceInfo&)));
    connect(m_deviceDiscovery, SIGNAL(linkManagerError(int)), this, SLOT(setError(int)));
    // startup the device discovery. Discovery results are obtained from signal connected above.
    TRAPD(errorCode, m_deviceDiscovery->StartDiscoveryL(inquiryTypeToIAC(inquiryType)))
    setError(errorCode, "KErrNotReady"); // user should call stop / or wait to get by this.
}

void QBluetoothDeviceDiscoveryAgentPrivateSymbian::stop()
{
    disconnect(m_deviceDiscovery);
    delete m_deviceDiscovery;
    m_deviceDiscovery = NULL;
}

bool QBluetoothDeviceDiscoveryAgentPrivateSymbian::isActive() const
{
    bool returnValue = false;
    if (m_deviceDiscovery)
        returnValue =  m_deviceDiscovery->IsActive();

    return returnValue;
}
void QBluetoothDeviceDiscoveryAgentPrivateSymbian::setError(int errorCode)
{
    setError(errorCode, QString());
}
void QBluetoothDeviceDiscoveryAgentPrivateSymbian::setError(int errorCode, QString errorDescription)
{
    QString errorString;
    //TODO missing error string from base classes
    switch (errorCode) {
        case KLinkManagerErrBase:
            errorString.append("Link manager base error value or Insufficient baseband resources error value");
            break;
        case KErrProxyWriteNotAvailable:
            errorString.append("Proxy write not available error value");
            break;
        case KErrReflexiveBluetoothLink:
            errorString.append("Reflexive BT link error value");
            break;
        case KErrPendingPhysicalLink:
            errorString.append("Physical link connection already pending when trying to connect the physical link");
            break;
        case KErrRemoteDeviceIndicatedNoBonding:
            errorString.append("Dedicated bonding attempt failure when the remote device responds with No-Bonding");
            break;
        default:
            break;
    }
    emit error(QBluetoothDeviceDiscoveryAgent::UnknownError);
}

void QBluetoothDeviceDiscoveryAgentPrivateSymbian::newDeviceFound(const QBluetoothDeviceInfo &device)
{
    // add found device to the list of devices
    discoveredDevices.append(device);
    emit deviceDiscovered(device);
}

uint QBluetoothDeviceDiscoveryAgentPrivateSymbian::inquiryTypeToIAC(const QBluetoothDeviceDiscoveryAgent::InquiryType type)
{
    if (type == QBluetoothDeviceDiscoveryAgent::GeneralUnlimitedInquiry)
        return KGIAC;
    else if (type == QBluetoothDeviceDiscoveryAgent::LimitedInquiry)
        return KLIAC;

    ASSERT(0); //enum has changed and not handled
    return KGIAC; // to get rid of compiler warning
}


QBluetoothDeviceDiscoveryAgentPrivate* QBluetoothDeviceDiscoveryAgentPrivate::constructPrivateObject(
    QBluetoothDeviceDiscoveryAgent *parent)
{
  QBluetoothDeviceDiscoveryAgentPrivateSymbian *d = new QBluetoothDeviceDiscoveryAgentPrivateSymbian(parent);
  d->q = parent;
  return d;
}

#include "moc_qbluetoothdevicediscoveryagent_symbian_p.cpp"

QTM_END_NAMESPACE
