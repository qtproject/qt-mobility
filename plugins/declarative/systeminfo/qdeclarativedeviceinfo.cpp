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


#include "qdeclarativedeviceinfo_p.h"
#include "qsystemdeviceinfo.h"
#include <QMetaType>
#include <QDebug>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemDeviceInfo, deviceInfo)

/*!
    \qmlclass DeviceInfo QDeclarativeDeviceInfo
    \brief The DeviceInfo element allows you to access information anbout the device and
        receive notifications from the device.

    \inherits QSystemDeviceInfo

    \ingroup qml-systeminfo

    This element is part of the \bold{QtMobility.systeminfo 1.1} module.
    It is a convience class to make QML usage easier.

    Note: To use notification signals, you need to set the monitor* properties to true.

\sa QSystemDeviceInfo
*/

/*!
    \qmlsignal DeviceInfo::batteryLevelChanged(int)

    This handler is called when battery level has changed.
*/
/*!
    \qmlsignal DeviceInfo::batteryStatusChanged(DeviceInfo::BatteryStatus)

    This handler is called when battery status has changed.

*/
/*!
    \qmlsignal DeviceInfo::powerStateChanged(DeviceINfo::PowerState)

    This handler is called when the power state has changed.
*/
/*!
    \qmlsignal DeviceInfo::currentProfileChanged(DeviceInfo::Profile)

    This handler is called when current device profile has changed.

*/
/*!
    \qmlsignal DeviceInfo::bluetoothStateChanged(bool)

    This handler is called when bluetooth power state has changed.

*/

/*!
    \qmlsignal DeviceInfo::wirelessKeyboardConnected(bool)

  This signal is emitted whenever a wireless keyboard is connected
*/

/*!
    \qmlsignal DeviceInfo::keyboardFlipped(bool)

      This signal is emitted whenever a phone flips open.
*/

/*!
    \qmlsignal DeviceInfo::deviceLocked(bool)

    This signal is emitted whenever the device lock state changes
*/

/*!
    \qmlsignal DeviceInfo::lockStatusChanged(DeviceInfo::LockTypeFlags)

    This signal is emitted whenever the lock state changes
*/


QDeclarativeDeviceInfo::QDeclarativeDeviceInfo(QObject *parent) :
    QSystemDeviceInfo(parent)
{
}

/*!
   This function starts the batteryLevelChanged notification

*/

void QDeclarativeDeviceInfo::startBatteryLevelChanged(bool on)
{
    monitoringBatteryLevel = on;
    if(on) {
        connect(deviceInfo(),SIGNAL(batteryLevelChanged(int)),
                this,SIGNAL(batteryLevelChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(batteryLevelChanged(int)),
                   this,SIGNAL(batteryLevelChanged(int)));
    }
}

/*!
    \qmlproperty bool DeviceInfo::monitorBatteryLevelChanges
    Use the monitorBatteryLevelChanges signal.

  */
bool QDeclarativeDeviceInfo::monitorBatteryLevelChanges()
{
    return monitoringBatteryLevel;
}

/*!
   This function starts the batteryStatusChanged notification

*/
void QDeclarativeDeviceInfo::startBatteryStatusChanged(bool on)
{
    monitoringBatteryStatus = on;
    if(on) {
    connect(deviceInfo(),SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
    }
}

/*!
    \qmlproperty bool DeviceInfo::monitorBatteryStatusChanges
    Use the monitorBatteryStatusChanges signal.
  */
bool QDeclarativeDeviceInfo::monitorBatteryStatusChanges()
{
    return monitoringBatteryStatus;
}


/*!
   This function starts the powerStateChanged notification

*/
void QDeclarativeDeviceInfo::startPowerStateChanged(bool on)
{
    monitoringPowerState = on;
    if(on) {
    connect(deviceInfo(),SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                this,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)));
    }
}

/*!
    \qmlproperty bool DeviceInfo::monitorPowerStateChanges
    Use the monitorPowerStateChanges signal.
  */
bool QDeclarativeDeviceInfo::monitorPowerStateChanges()
{
    return monitoringPowerState;
}


/*!
   This function is needed to start currentProfileChanged notification

*/
void QDeclarativeDeviceInfo::startCurrentProfileChanged(bool on)
{
    monitoringCurrentProfile = on;
    if(on) {
    connect(deviceInfo(),SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            this,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                this,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)));
    }
}

/*!
    \qmlproperty bool DeviceInfo::monitorCurrentProfileChanges
   Use the monitorCurrentProfileChanges signal.
  */
bool QDeclarativeDeviceInfo::monitorCurrentProfileChanges()
{
    return monitoringCurrentProfile;
}


/*!
   This function starts the bluetoothStateChanged notification

*/
void QDeclarativeDeviceInfo::startBluetoothStateChanged(bool on)
{
    monitorBluetoothState = on;
    if(on) {
    connect(deviceInfo(),SIGNAL(bluetoothStateChanged(bool)),
            this,SIGNAL(bluetoothStateChanged(bool)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(bluetoothStateChanged(bool)),
                this,SIGNAL(bluetoothStateChanged(bool)));
    }
}

/*!
    \qmlproperty bool DeviceInfo::monitorBluetoothStateChanges
    Use the bluetoothStateChanges signal.
  */
bool QDeclarativeDeviceInfo::monitorBluetoothStateChanges()
{
   return monitorBluetoothState;
}

/*!

  \qmlproperty bool DeviceInfo::monitorWirelessKeyboardConnects()

  Start the connection for the wirelessKeyboardConnected signal.
  */
bool QDeclarativeDeviceInfo::monitorWirelessKeyboardConnects()
{
 return monitoringWirelessKeyboard;
}


/*!
   This function starts the startBatteryStatusChanged notification.

*/
void QDeclarativeDeviceInfo::startWirelessKeyboardConnected(bool on)
{
    monitoringWirelessKeyboard = on;
    if(on) {
        connect(deviceInfo(),SIGNAL(wirelessKeyboardConnected(bool connected)),
                this,SIGNAL(wirelessKeyboardConnected(bool connected)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(wirelessKeyboardConnected(bool connected)),
                this,SIGNAL(wirelessKeyboardConnected(bool connected)));
    }
}


/*!

  \qmlproperty bool DeviceInfo::monitorKeyboardFlips()

  Start the connection for the keyboardFlipped signal.
  */
bool QDeclarativeDeviceInfo::monitorKeyboardFlips()
{
    return monitoringKeyboardFlipped;
}

/*!
   This function starts the startKeyboardFlipped notification.

*/
void QDeclarativeDeviceInfo::startKeyboardFlipped(bool on)
{
    monitoringKeyboardFlipped = on;
    if(on) {
        connect(deviceInfo(),SIGNAL(keyboardFlipped(bool open)),
                this,SIGNAL(keyboardFlipped(bool open)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(keyboardFlipped(bool open)),
                this,SIGNAL(keyboardFlipped(bool open)));
    }
}


/*!

  \qmlproperty bool Device::monitorDeviceLocks()

  Start the connection for the deviceLocked signal.
  */
bool QDeclarativeDeviceInfo::monitorDeviceLocks()
{
    return monitoringDeviceLocked;
}

/*!
   This function starts the startDeviceLocked notification.

*/
void QDeclarativeDeviceInfo::startDeviceLocked(bool on)
{
    monitoringDeviceLocked = on;
    if(on) {
        connect(deviceInfo(),SIGNAL(deviceLocked(bool isLocked)),
                this,SIGNAL(deviceLocked(bool isLocked)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(deviceLocked(bool isLocked)),
                this,SIGNAL(deviceLocked(bool isLocked)));
    }
}

/*!

  \qmlproperty bool DeviceInfo::monitorLockStatusChanges()

  Start the connection for the lockStatusChanged signal.
  */

bool QDeclarativeDeviceInfo::monitorLockStatusChanges()
{
    return monitoringLockStatusChanged;
}

/*!
   This function starts the startLockStatusChanged notification.

*/
void QDeclarativeDeviceInfo::startLockStatusChanged(bool on)
{
    monitoringLockStatusChanged = on;
    if(on) {
        connect(deviceInfo(),SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)),
                this,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)),Qt::UniqueConnection);
    } else {
        disconnect(deviceInfo(),SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)),
                this,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)));
    }
}

/*!
  \qmlproperty QString DeviceInfo::imei

     Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
  */

/*!
  \qmlproperty QString DeviceInfo::imsi
 Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none
  */

/*!
  \qmlproperty QString DeviceInfo::manufacturer
    Returns the name of the manufacturer of this device. In the case of desktops, the name of the vendor
    of the motherboard.
  */
/*!
  \qmlproperty QString DeviceInfo::model
    Returns the model information of the device. In the case of desktops where no
    model information is present, the CPU architect, such as i686, and machine type, such as Server,
    Desktop or Laptop.
  */
/*!
  \qmlproperty QString DeviceInfo::productName
    Returns the product name of the device. In the case where no product information is available, an empty string will be returned.

  */
/*!
  \qmlproperty int DeviceInfo::batteryLevel
   Returns the battery charge level as percentage 1 - 100 scale.
  */
/*!
  \qmlproperty InputMethodFlags DeviceInfo::inputMethodType
Returns the QSystemDeviceInfo::InputMethodFlags InputMethodType that the system uses.
  */
/*!
  \qmlproperty BatteryStatus DeviceInfo::batteryStatus
  Returns the battery charge status.
  */
/*!
  \qmlproperty bool DeviceInfo::isDeviceLocked

  Returns true if the device is locked, otherwise false.
  */
/*!
  \qmlproperty SimStatus DeviceInfo::simStatus
  Returns the QSystemDeviceInfo::simStatus status of SIM card.
  */
/*!
  \qmlproperty Profile DeviceInfo::currentProfile
  Gets the current QSystemDeviceInfo::currentProfile device profile.
  */
/*!
  \qmlproperty PowerState DeviceInfo::currentPowerState

  Gets the current QSystemDeviceInfo::currentPowerState state.
  */
/*!
  \qmlproperty bool DeviceInfo::currentBluetoothPowerState
Gets the current bluetooth power state.
  */
/*!
  \qmlproperty KeyboardTypeFlags DeviceInfo::keyboardTypes
  Returns the type of keyboards found.
  */
/*!
  \qmlproperty bool DeviceInfo::isWirelessKeyboardConnected
  Returns true if a wireless keyboard is connected, otherwise false;
  */
/*!
  \qmlproperty bool DeviceInfo::isKeyboardFlippedOpen
  Returns true if the flip keyboard is open, otherwise false;
  */
/*!
  \qmlproperty QUuid DeviceInfo::uniqueDeviceID
  Returns a unique identifier for the machine.

  Depending on security enforcement on platform, this may return a non unique number, or 0.

  */

/*!
  \qmlproperty LockTypeFlags DeviceInfo::lockStatus
  Returns the QSystemDeviceInfo::LockTypeFlags type of lock state the device might be in.
  The LockType must be currently active not just enabled.
  */
/*!
  \qmlmethod ProfileDetails DeviceInfo::activeProfileDetails
  Returns a QSystemDeviceInfo::ProfileDetails for the currently active profile.
  */

/*!
  \qmlmethod bool DeviceInfo::keypadLightOn(QSystemDeviceInfo::KeypadType type)
  Returns true if the key pad, indicated by \a type, light is on, otherwise false;
  */
