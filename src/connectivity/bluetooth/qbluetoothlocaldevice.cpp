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

#include "qbluetoothlocaldevice.h"
#include "qbluetoothaddress.h"

#include <QtCore/QString>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

/*!
    \class QBluetoothLocalDevice
    \brief The QBluetoothLocalDevice class provides access to local Bluetooth devices.

    \ingroup connectivity-bluetooth
    \inmodule QtConnectivity

    QBluetoothLocalDevice provides functions for getting and setting the state of local Bluetooth
    devices.
*/

/*!
    \enum QBluetoothLocalDevice::Pairing

    This enum describes the pairing state between two Bluetooth devices.

    \value Unpaired         The Bluetooth devices are not paired.
    \value Paired           The Bluetooth devices are paired. The system will prompt the user for
                            authorization when the remote device initiates a connection to the
                            local device.
    \value AuthorizedPaired The Bluetooth devices are paired. The system will not prompt the user
                            for authorization when the remote device initiates a connection to the
                            local device.
*/

/*!
    \enum QBluetoothLocalDevice::PowerState

    This enum describes the power state of the local Bluetooth device.

    \value PowerOn  The device is powered on.
    \value PowerOff The device is powered off.
*/

/*!
    \enum QBluetoothLocalDevice::HostMode

    This enum describes the most of the local Bluetooth device.

    \value HostUnconnectable    Remove Bluetooth devices cannot connect to the local Bluetooth
                                device.
    \value HostConnectable      Remote Bluetooth devices can connect to the local Bluetooth device
                                if they have previously been paired with it or otherwise know its
                                address.
    \value HostDiscoverable     Remote Bluetooth devices can discover the presense of the local
                                Bluetooth device.
*/

/*!
    Constructs a QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::QBluetoothLocalDevice()
{
}

/*!
    Destroys the QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::~QBluetoothLocalDevice()
{
}

/*!
    Returns true the QBluetoothLocalDevice represents an available local Bluetooth device;
    otherwise return false.
*/
bool QBluetoothLocalDevice::isValid() const
{
    Q_D(const QBluetoothLocalDevice);
    return d;
}

/*!
    Sets pairing of this local Bluetooth device and the remote Bluetooth device with \a address to
    \a pairing.
*/
void QBluetoothLocalDevice::setPairing(const QBluetoothAddress &address, Pairing pairing)
{
}

/*!
    Returns the current pairing between this local Bluetooth device and the remote Bluetooth device
    with \a address.
*/
QBluetoothLocalDevice::Pairing QBluetoothLocalDevice::pairing(const QBluetoothAddress &address) const
{
    return Unpaired;
}

/*!
    \fn void QBluetoothLocalDevice::setPowerState(PowerState powerState)

    Sets the power state of this local Bluetooth device to \a powerState.
*/

/*!
    \fn QBluetoothLocalDevice::PowerState QBluetoothLocalDevice::powerState() const

    Returns the current power state of this local Bluetooth device.
*/

/*!
    \fn void QBluetoothLocalDevice::setHostMode(QBluetoothLocalDevice::HostMode mode)

    Sets the host mode the this local Bluetooth device to \a mode.
*/

/*!
    \fn QBluetoothLocalDevice::HostMode QBluetoothLocalDevice::hostMode() const

    Returns the current host mode of this local Bluetooth device.
*/

/*!
    \fn QBluetoothLocalDevice::name() const

    Returns the name of this local Bluetooth device.
*/

/*!
    \fn QBluetoothLocalDevice::address() const

    Returns the address of this local Bluetooth device.
*/

/*!
    \fn QBluetoothLocalDevice QBluetoothLocalDevice::defaultDevice()

    Returns the default local Bluetooth device.
*/

/*!
    \fn QList<QBluetoothLocalDevice> QBluetoothLocalDevice::allDevices()

    Returns a list of all available local Bluetooth devices.
*/

QTM_END_NAMESPACE

QT_END_HEADER
