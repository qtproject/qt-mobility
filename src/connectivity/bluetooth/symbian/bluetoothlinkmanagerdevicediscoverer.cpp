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

// INCLUDE FILES
#include "bluetoothlinkmanagerdevicediscoverer.h"
#include "qbluetoothaddress.h"
#include "qbluetoothdeviceinfo.h"
#include <qstring.h>

#include "utils_symbian_p.h"
#include <qdebug.h>

QTM_BEGIN_NAMESPACE

/*!
    \class BluetoothLinkManagerDeviceDiscoverer
    \brief The BluetoothLinkManagerDeviceDiscoverer is class searches other bluetooth devices.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    BluetoothLinkManagerDeviceDiscoverer is an Symbian ActiveObject derived class that discovers
    other Bluetooth devices using "BTLinkManager" protocol.

    When Bluetoothdevices are found signal deviceDiscovered(QBluetoothDeviceInfo &) is emitted. When
    all devices are found (or none) signal deviceDiscoveryComplete() is emitted.

    In case of an error signal linkManagerError(int error) is emitted. In the case of errorSignal
    one should consider deleting this class and creating it again.
*/

_LIT(KBTLinkManagerTxt,"BTLinkManager");

BluetoothLinkManagerDeviceDiscoverer::BluetoothLinkManagerDeviceDiscoverer(RSocketServ &socketServer,QObject *parent)
    : QObject(parent)
    , CActive(CActive::EPriorityStandard)
    , m_socketServer(socketServer)
{
    TInt result;

    /* find Bluetooth protocol */
    TProtocolDesc protocol;
    result = m_socketServer.FindProtocol(KBTLinkManagerTxt(),protocol);
    if (result != KErrNone) {
        setError(result);
    }
    /* Create and initialise an RHostResolver */
    result = m_hostResolver.Open(m_socketServer, protocol.iAddrFamily, protocol.iProtocol);
    if (result != KErrNone) {
        setError(result);
    }

    //add this active object to scheduler
    CActiveScheduler::Add(this);
}

BluetoothLinkManagerDeviceDiscoverer::~BluetoothLinkManagerDeviceDiscoverer()
{
    // cancel active object
    Cancel();
    m_hostResolver.Close();
}
/*!
    Starts up device discovery. When devices are discovered signal deviceDiscovered is emitted.
    After signal deviceDiscoveryComplete() is emitted new discovery request can be made
    Leaves with KErrNotReady if discovery is ongoing and new discovery is started

*/
void BluetoothLinkManagerDeviceDiscoverer::StartDiscoveryL(const uint discoveryType)
{
    if (!IsActive()) {
        m_addr.SetIAC( discoveryType );
        m_addr.SetAction(KHostResInquiry | KHostResName | KHostResIgnoreCache);
        m_hostResolver.GetByAddress(m_addr, m_entry, iStatus);
        SetActive();
    }
    else {
        User::Leave(KErrNotReady);
    }
}
/*!
  Informs that our request has been prosessed and the data is available to be used.
*/
void BluetoothLinkManagerDeviceDiscoverer::RunL()
{
    if (iStatus.Int() != KErrNone)
        emit deviceDiscoveryComplete(iStatus.Int());
    else {
        emit deviceDiscovered(currentDeviceDataToQBluetoothDeviceInfo());
        // get next (possible) discovered device
        m_hostResolver.Next(m_entry, iStatus);
        // set this AO active ie running
        SetActive();
    }
}
/*!
  Cancel current request.
*/
void BluetoothLinkManagerDeviceDiscoverer::DoCancel()
{
    m_hostResolver.Cancel();
}
/*!
  Stops the discovery. Signal deviceDiscoveryComplete(int) with KErrCancel might be emitted.
*/
void BluetoothLinkManagerDeviceDiscoverer::Stop()
{
    Cancel();
}

/*!
    Transforms Symbian device name, address and service classes to QBluetootDeviceInfo.
*/
QBluetoothDeviceInfo BluetoothLinkManagerDeviceDiscoverer::currentDeviceDataToQBluetoothDeviceInfo() const
{
    // extract device information from results and map them to QBluetoothDeviceInfo
    // device name
    THostName symbianDeviceName = m_entry().iName;
    QString deviceName = QString::fromUtf16(symbianDeviceName.Ptr(), symbianDeviceName.Length()).toUpper();

    // unique address of the device
    const TBTDevAddr symbianDeviceAddress = static_cast<TBTSockAddr> (m_entry().iAddr).BTAddr();
    QBluetoothAddress bluetoothAddress = qTBTDevAddrToQBluetoothAddress(symbianDeviceAddress);

    // format symbian major/minor numbers
    quint32 deviceClass = qTPackSymbianDeviceClass(m_addr);

    QBluetoothDeviceInfo deviceInfo(bluetoothAddress, deviceName, deviceClass);
    qDebug()<< "Discovered device: name="<< deviceName <<", address=" << bluetoothAddress.toString() <<", class=" << deviceClass;
    return deviceInfo;
}
void BluetoothLinkManagerDeviceDiscoverer::setError(int errorCode)
{
    QString errorDescription;
    switch (errorCode) {
        case KLinkManagerErrBase:
            errorDescription.append("Link manager base error value or Insufficient baseband resources error value");
            break;
        case KErrProxyWriteNotAvailable:
            errorDescription.append("Proxy write not available error value");
            break;
        case KErrReflexiveBluetoothLink:
            errorDescription.append("Reflexive BT link error value");
            break;
        case KErrPendingPhysicalLink:
            errorDescription.append("Physical link connection already pending when trying to connect the physical link");
            break;
        case KErrRemoteDeviceIndicatedNoBonding:
            errorDescription.append("Dedicated bonding attempt failure when the remote device responds with No-Bonding");
            break;
        case KErrNotReady:
            errorDescription.append("KErrNotReady");
        default:
            break;
    }
    if (errorCode == KErrCancel)
        emit linkManagerError(QBluetoothDeviceDiscoveryAgent::Canceled);
    else if (errorCode != KErrNone)
        emit linkManagerError(QBluetoothDeviceDiscoveryAgent::UnknownError);
}
#include "moc_bluetoothlinkmanagerdevicediscoverer.cpp"

QTM_END_NAMESPACE
