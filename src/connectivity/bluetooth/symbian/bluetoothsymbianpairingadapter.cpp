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
#include "bluetoothsymbianpairingadapter.h"
#include "qbluetoothaddress.h"
#include "qbluetoothdeviceinfo.h"
#include "qbluetoothlocaldevice.h"
#include <qstring.h>
#include <btdevice.h>

#include "utils_symbian_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class BluetoothSymbianPairingAdapter
    \brief The BluetoothSymbianPairingAdapter is an adapter class for bluetooth pairing functinality.

    The BluetoothSymbianPairingAdapter is constructed to use for a one QBluetoothAddress.
    It uses Following Symbian classes CBTEngConnMan and CBTEngDevMan.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

*/

BluetoothSymbianPairingAdapter::BluetoothSymbianPairingAdapter(const QBluetoothAddress &address, QObject *parent)
    : QObject(parent)
    , m_pairingEngine(0)
    , m_bluetoothDeviceManager(0)
    , m_address(address)
    , m_pairingStatus(QBluetoothLocalDevice::Unpaired)
    , m_pairingOngoing(false)
    , m_removeOngoing(false)
{
    TRAP(m_errorCode,
        m_pairingEngine = CBTEngConnMan::NewL(this);
        m_bluetoothDeviceManager = CBTEngDevMan::NewL(this);
        )
    if (m_errorCode == KErrNone)
        setRemoteDevicePairingStatusFromRegistry();
    else {
        emit pairingError(m_errorCode);
    }
}
BluetoothSymbianPairingAdapter::~BluetoothSymbianPairingAdapter()
{
    delete m_pairingEngine;
    delete m_bluetoothDeviceManager;
}

int BluetoothSymbianPairingAdapter::errorCode() const
{
    return m_errorCode;
}

QString BluetoothSymbianPairingAdapter::pairingErrorString() const
{
    return m_pairingErrorString;
}

QBluetoothLocalDevice::Pairing BluetoothSymbianPairingAdapter::remoteDevicePairingStatus()
{
    QBluetoothLocalDevice::Pairing returnValue = QBluetoothLocalDevice::Unpaired;
    // if the address provided is not valid
    if (m_address.isNull()) {
        m_pairingStatus = QBluetoothLocalDevice::Unpaired;
        emit pairingError(KErrArgument);
        return returnValue;
    }
    int errorCode = 0;
    // use Symbian registry to get a current pairing status of a device
    // do not pass observer for this method -> use sync version of GetDevices()
    CBTEngDevMan *btDeviceManager = NULL;
    TRAPD(deviceManagerCreationErr, btDeviceManager = CBTEngDevMan::NewL(NULL));
    if (deviceManagerCreationErr != KErrNone) {
        emit pairingError(deviceManagerCreationErr);
        return returnValue;
    }
    CleanupStack::PushL(btDeviceManager);

    // Spesify search criteria
    TBTRegistrySearch lSearchCriteria;
    TBTDevAddr btAddress(m_address.toUInt64());
    lSearchCriteria.FindAddress(btAddress);

    CBTDeviceArray *listOfDevices = NULL;
    TRAPD(arrayCreationErr,listOfDevices = createDeviceArrayL());
    if (arrayCreationErr !=KErrNone) {
        emit pairingError(arrayCreationErr);
        // cleanup manager before returning
        CleanupStack::Pop(btDeviceManager);
        delete btDeviceManager;
        return returnValue;
    }

    CleanupStack::PushL(listOfDevices);
    m_errorCode = btDeviceManager->GetDevices(lSearchCriteria,listOfDevices);
    if (errorCode != KErrNone) {
        emit pairingError(errorCode);
        return returnValue;
    }
    if ( listOfDevices->Count() == 1) {
        CBTDevice *device= listOfDevices->At(0);
        bool isValidDevicePaired = device->IsValidPaired();
        bool isDevicePaired = device->IsPaired();
        //TODO Check wheter check is correct?
        if (isValidDevicePaired || isDevicePaired)
            returnValue = QBluetoothLocalDevice::Paired;
        else if (!isValidDevicePaired && !isDevicePaired)
            returnValue = QBluetoothLocalDevice::Unpaired;

    } else {
        returnValue = QBluetoothLocalDevice::Unpaired;
    }
    //cleanup
    CleanupStack::Pop(listOfDevices);
    listOfDevices->ResetAndDestroy();
    delete listOfDevices;
    CleanupStack::Pop(btDeviceManager);
    delete btDeviceManager;
    return returnValue;
}
CBTDeviceArray* BluetoothSymbianPairingAdapter::createDeviceArrayL()
{
    return q_check_ptr(new CBTDeviceArray(10));
}

void BluetoothSymbianPairingAdapter::setRemoteDevicePairingStatusFromRegistry()
{
    m_pairingStatus = remoteDevicePairingStatus();
}

void BluetoothSymbianPairingAdapter::startPairing(QBluetoothLocalDevice::Pairing pairing)
{
    TBTDevAddr btAddress(m_address.toUInt64());

    // start async pairing process
    m_pairingOngoing = true;
    int error = m_pairingEngine->PairDevice(btAddress);
    if (error != KErrNone) {
        PairingComplete(btAddress, error);
    }
}
void BluetoothSymbianPairingAdapter::removePairing()
{
    // setup current values
    int errorCode = 0;
    m_removeOngoing = true;

    // Spesify search criteria
    TBTRegistrySearch searchCriteria;
    TBTDevAddr btAddress(m_address.toUInt64());
    searchCriteria.FindAddress(btAddress);
    errorCode = m_bluetoothDeviceManager->DeleteDevices(searchCriteria);
    // if there is error then manually call callback for error routines
    if (errorCode != KErrNone)
        HandleDevManComplete(errorCode);

}

QBluetoothLocalDevice::Pairing BluetoothSymbianPairingAdapter::pairingStatus() const
{
    return m_pairingStatus;
}

void BluetoothSymbianPairingAdapter::ConnectComplete( TBTDevAddr& aAddr, TInt aErr,
                                       RBTDevAddrArray* aConflicts )
{
    Q_UNUSED(aAddr);
    Q_UNUSED(aErr);
    Q_UNUSED(aConflicts);
}

void BluetoothSymbianPairingAdapter::DisconnectComplete( TBTDevAddr& aAddr, TInt aErr )
{
    Q_UNUSED(aAddr);
    Q_UNUSED(aErr);
}

void BluetoothSymbianPairingAdapter::PairingComplete( TBTDevAddr& aAddr, TInt aErr )
{
    m_pairingErrorString = QString();
    m_pairingOngoing = false;
    switch (aErr) {
        case KErrNone:
            // TODO: optimization
            setRemoteDevicePairingStatusFromRegistry();
            emit pairingFinished(qTBTDevAddrToQBluetoothAddress(aAddr),m_pairingStatus);
            break;
        case KErrRemoteDeviceIndicatedNoBonding:
            m_pairingErrorString.append("Dedicated bonding attempt failure when the remote device responds with No-Bonding");
            break;
        default:
            m_pairingErrorString.append("Symbian pairing error=") + aErr;
        break;
    }
    if (aErr != KErrNone)
        emit pairingError(aErr);

}
void BluetoothSymbianPairingAdapter::HandleDevManComplete( TInt aErr )
{
    if (aErr != KErrNone)
        emit pairingError(aErr);

    if (m_removeOngoing) {
        m_removeOngoing = false;
        m_pairingStatus = QBluetoothLocalDevice::Unpaired;
        emit pairingFinished(m_address,m_pairingStatus);
    }
}
void BluetoothSymbianPairingAdapter::HandleGetDevicesComplete( TInt aErr,CBTDeviceArray* aDeviceArray )
{
    Q_UNUSED(aErr);
    Q_UNUSED(aDeviceArray);
}

#include "moc_bluetoothsymbianpairingadapter.cpp"

QTM_END_NAMESPACE
