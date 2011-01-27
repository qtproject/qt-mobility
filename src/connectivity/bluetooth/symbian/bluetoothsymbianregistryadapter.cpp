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
#include "bluetoothsymbianregistryadapter.h"
#include "qbluetoothaddress.h"
#include "qbluetoothdeviceinfo.h"
#include <qstring.h>
#include <btdevice.h>
#include "utils_symbian_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class BluetoothSymbianRegistryAdapter
    \brief The BluetoothSymbianRegistryAdapter is an adapter class for handling Symbian Bluetooth
    Registry.

    The BluetoothSymbianRegistryAdapter is constructed to use for a one QBluetoothAddress.
    It uses following Symbian class CBTEngDevMan for native operations.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

*/

BluetoothSymbianRegistryAdapter::BluetoothSymbianRegistryAdapter(const QBluetoothAddress &address, QObject *parent)
    : QObject(parent)
    , m_bluetoothDeviceManager(0)
    , m_address(address)
    , m_pairingStatus(QBluetoothLocalDevice::Unpaired)
{
    TRAP(m_errorCode, m_bluetoothDeviceManager = CBTEngDevMan::NewL(this))

    if (m_errorCode == KErrNone)
        m_pairingStatus = remoteDevicePairingStatus();
    else
        emit registryHandlingError(m_errorCode);
}
BluetoothSymbianRegistryAdapter::~BluetoothSymbianRegistryAdapter()
{
    delete m_bluetoothDeviceManager;
}

int BluetoothSymbianRegistryAdapter::errorCode() const
{
    return m_errorCode;
}

QString BluetoothSymbianRegistryAdapter::pairingErrorString() const
{
    return m_pairingErrorString;
}

QBluetoothLocalDevice::Pairing BluetoothSymbianRegistryAdapter::remoteDevicePairingStatus()
{
    QBluetoothLocalDevice::Pairing returnValue = QBluetoothLocalDevice::Unpaired;
    // if the address provided is not valid
    if (m_address.isNull()) {
        emit registryHandlingError((int)KErrArgument);
        return returnValue;
    }
    int errorCode = 0;
    // use Symbian registry to get a current pairing status of a device
    // do not pass observer for this method -> use sync version of GetDevices()
    CBTEngDevMan *btDeviceManager = NULL;
    TRAPD(deviceManagerCreationErr, btDeviceManager = CBTEngDevMan::NewL(NULL));
    if (deviceManagerCreationErr != KErrNone) {
        emit registryHandlingError((int)deviceManagerCreationErr);
        return returnValue;
    }

    // Spesify search criteria
    TBTRegistrySearch lSearchCriteria;
    TBTDevAddr btAddress(m_address.toUInt64());
    lSearchCriteria.FindAddress(btAddress);

    CBTDeviceArray *listOfDevices = NULL;
    TRAPD(arrayCreationErr,listOfDevices = createDeviceArrayL());
    if (arrayCreationErr !=KErrNone) {
        emit registryHandlingError((int)arrayCreationErr);
        // cleanup manager before returning
        delete btDeviceManager;
        return returnValue;
    }
    errorCode = btDeviceManager->GetDevices(lSearchCriteria,listOfDevices);
    if (errorCode != KErrNone) {
        emit registryHandlingError(errorCode);
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
    listOfDevices->ResetAndDestroy();
    delete listOfDevices;
    delete btDeviceManager;
    return returnValue;
}
CBTDeviceArray* BluetoothSymbianRegistryAdapter::createDeviceArrayL() const
{
    return q_check_ptr(new CBTDeviceArray(10));
}

void BluetoothSymbianRegistryAdapter::setRemoteDevicePairingStatusFromRegistry()
{
    QBluetoothLocalDevice::Pairing currentPairingStatus = remoteDevicePairingStatus();
    // check current pairing status from Symbian Bluetooth registry
    if (currentPairingStatus != m_pairingStatus) {
        m_pairingStatus = currentPairingStatus;
        emit pairingStatusChanged(m_address, m_pairingStatus);
    }
    else
        currentPairingStatus = m_pairingStatus;
}

void BluetoothSymbianRegistryAdapter::removePairing()
{
    // setup current values
    int errorCode = 0;

    // Spesify search criteria
    TBTRegistrySearch searchCriteria;
    TBTDevAddr btAddress(m_address.toUInt64());
    searchCriteria.FindAddress(btAddress);
    errorCode = m_bluetoothDeviceManager->DeleteDevices(searchCriteria);
    // if there is error then manually call callback for error routines
    if (errorCode != KErrNone)
        HandleDevManComplete(errorCode);

}

QBluetoothLocalDevice::Pairing BluetoothSymbianRegistryAdapter::pairingStatus() const
{
    return m_pairingStatus;
}

void BluetoothSymbianRegistryAdapter::HandleDevManComplete( TInt aErr )
{
    m_errorCode = aErr;
    if (aErr != KErrNone)
        emit registryHandlingError(aErr);

    emit operationFinished(m_address);

}

void BluetoothSymbianRegistryAdapter::HandleGetDevicesComplete( TInt aErr,CBTDeviceArray* aDeviceArray )
{
    Q_UNUSED(aErr);
    Q_UNUSED(aDeviceArray);
}

#include "moc_bluetoothsymbianregistryadapter.cpp"

QTM_END_NAMESPACE
