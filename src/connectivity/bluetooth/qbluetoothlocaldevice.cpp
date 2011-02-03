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
//#ifdef QTM_SYMBIAN_BLUETOOTH
#include "qbluetoothlocaldevice_p.h"
//#endif //QTM_SYMBIAN_BLUETOOTH
#include "qbluetoothaddress.h"

#include <QtCore/QString>
#include <QDebug>

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
  \enum QBluetoothLocalDevice::Error

  This enum describes errors that maybe returned

  \value NoError        No known error
  \value PairingError   Error in pairing
  \value UnknownError   Unknown error

*/

/*!
    \enum QBluetoothLocalDevice::HostMode

    This enum describes the most of the local Bluetooth device.

    \value HostPoweredOff       Powers the device down
    \value HostConnectable      Remote Bluetooth devices can connect to the local Bluetooth device
                                if they have previously been paired with it or otherwise know its
                                address. This powers up the device if it was powered off.
    \value HostDiscoverable     Remote Bluetooth devices can discover the presence of the local
                                Bluetooth device.  The device will also be connectable, and powered on.
*/

/*!
    Destroys the QBluetoothLocalDevice.
*/
QBluetoothLocalDevice::~QBluetoothLocalDevice()
{
    delete d_ptr;
}

/*!
    Returns true the QBluetoothLocalDevice represents an available local Bluetooth device;
    otherwise return false.
*/
bool QBluetoothLocalDevice::isValid() const
{    
    return d_ptr;
}

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
    \fn QList<QBluetoothLocalDevice> QBluetoothLocalDevice::allDevices()

    Returns a list of all available local Bluetooth devices.
*/

/*!
  \fn void QBluetoothLocalDevice::powerOn()

  Powers the device on returning it to the hostMode() state is was in when powered down
*/

/*!
  \fn QBluetoothLocalDevice::QBluetoothLocalDevice(QObject *parent)
    Constructs a QBluetoothLocalDevice with \a parent.
*/

/*!
  \fn QBluetoothLocalDevice::hostModeStateChanged(QBluetoothLocalDevice::HostMode state)
  The \a state of the host has transitioned to a different HostMode
*/


/*!
  \fn QBluetoothLocalDevice::pairingDisplayConfirmation(const QBluetoothAddress &address, QString pin)
  Signal by some platforms to display a pairing confirmation dialog for \a address.  The user
  is asked to confirm the \a pin is the same on both devices.  QBluetoothLocalDevice::pairingConfirmation(bool)
  must be called to indicate if the user accepts or rejects the displayed pin.
*/

/*!
  \fn QBluetoothLocalDevice::pairingConfirmation(bool accept)
  To be called after getting a pairingDisplayConfirmation().  The \a accept parameter either
  accepts the pairing or rejects it.
*/

/*!
  \fn QBluetoothLocalDevice::pairingDisplayPinCode(const QBluetoothAddress &address, QString pin)
  Signal by some platforms to display the \a pin to the user for \a address.  The pin is automatically
  generated, and does not need to be confirmed.
*/

/*!
  \fn QBluetoothLocalDevice::requestPairing(const QBluetoothAddress &address, Pairing pairing)
  Set the \a pairing status with \a address.  The results are returned via
  the signal pairingFinished().  Caution: creating a pairing may take minutes, and can require
  the user to acknowledge dialogs.
*/

/*!
    \fn QBluetoothLocalDevice::pairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
  Pairing has completed with \a address. Current pairing status is in \a pairing.
*/

/*!
  \fn QBluetoothLocalDevice::error(QBluetoothLocalDevice::Error error)
  Signal emited for pairing if there's an exceptional \a error
*/


/*!
  \fn QBluetoothLocalDevice::QBluetoothLocalDevice(const QBluetoothAddress &address, QObject *parent = 0)

  Construct new QBluetoothLocalDevice for \a address.
*/


#include "moc_qbluetoothlocaldevice.cpp"

QTM_END_NAMESPACE
