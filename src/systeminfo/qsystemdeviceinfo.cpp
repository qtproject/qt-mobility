/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsystemdeviceinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE
        Q_GLOBAL_STATIC(QSystemDeviceInfoPrivate, deviceInfoPrivate)

#ifdef QT_SIMULATOR
QSystemDeviceInfoPrivate *getSystemDeviceInfoPrivate() { return deviceInfoPrivate(); }
#endif

// device
        /*!
        \class QSystemDeviceInfo
        \ingroup systeminfo
        \inmodule QtSystemInfo
        \brief The QSystemDeviceInfo class provides access to device information from the system.
        */

        /*!
          \fn void QSystemDeviceInfo::batteryLevelChanged(int level)

          This signal is emitted when battery level has changed.
          \a level is the new level.
         */

        /*!
          \fn void QSystemDeviceInfo::batteryStatusChanged(QSystemDeviceInfo::BatteryStatus status)

          This signal is emitted when battery status has changed.
          \a status is the new status.
         */

           /*!
          \fn void QSystemDeviceInfo::powerStateChanged(QSystemDeviceInfo::PowerState state)

          This signal is emitted when the power state has changed, such as when a phone gets plugged in to the wall.
          \a state is the new power state.
         */

        /*!
          \fn  void QSystemDeviceInfo::currentProfileChanged(QSystemDeviceInfo::Profile profile)

          This signal is emitted whenever the users active profile changes, specified by \a profile.
        */


        /*!
            \enum QSystemDeviceInfo::BatteryStatus
            This enum describes the status of the main battery.

            \value NoBatteryLevel          Battery level undetermined.
            \value BatteryCritical         Battery level is critical 3% or less.
            \value BatteryVeryLow          Battery level is very low, 10% or less.
            \value BatteryLow              Battery level is low 40% or less.
            \value BatteryNormal           Battery level is above 40%.

          */
        /*!
            \enum QSystemDeviceInfo::PowerState
            This enum describes the power state:

            \value UnknownPower                   Power error.
            \value BatteryPower                   On battery power.
            \value WallPower                      On wall power.
            \value WallPowerChargingBattery       On wall power and charging main battery.

          */
        /*!
            \enum QSystemDeviceInfo::Profile
            This enum describes the current operating profile of the device or computer.

            \value UnknownProfile          Profile unknown or error.
            \value SilentProfile           Silent profile.
            \value NormalProfile           Normal profile.
            \value LoudProfile             Loud profile.
            \value VibProfile              Vibrate profile.
            \value OfflineProfile          Offline profile.
            \value PowersaveProfile        Powersave profile.
            \value BeepProfile             Beep profile.
            \value CustomProfile           Custom profile.

          */

        /*!
            \enum QSystemDeviceInfo::SimStatus
            This enum describes the status is the sim card or cards.

            \value SimNotAvailable         SIM is not available on this device.
            \value SingleSimAvailable         One SIM card is available on this.
            \value DualSimAvailable           Two SIM cards are available on this device.
            \value SimLocked                  Device has SIM lock enabled.
        */

        /*!
            \enum QSystemDeviceInfo::InputMethod
            This enum describes the device method of user input.

            \value Keys               Device has key/buttons.
            \value Keypad             Device has keypad (1,2,3, etc).
            \value Keyboard           Device has qwerty keyboard.
            \value SingleTouch        Device has single touch screen.
            \value MultiTouch         Device has multi-point touch screen.
            \value Mouse              Device has a mouse.
        */

        /*!
            \enum QSystemDeviceInfo::KeyboardType
            This enum describes the type of keyboard.

            \value UnknownKeyboard             Error or unknown keyboard type..
            \value SoftwareKeyboard            Software Keyboard.
            \value ITUKeypad                   Standard phone keyboard.
            \value HalfQwertyKeyboard          Half qwerty keboard like on Nokia E55.
            \value FullQwertyKeyboard          Standard qwerty type keyboard.
            \value WirelessKeyboard            Bluetooth or other wireless keyboard.
        */
/*!
          \enum QSystemDeviceInfo::keypadType
          This enum describe the type of keypad/keyboard.

          \value PrimaryKeypad                 Primary keypad or keyboard used.
          \value SecondaryKeypad               Secondary keypad or keyboard used.
          */

        /*!
          \enum QSystemDeviceInfo::LockType
          This enum describes the type of lock.

          \value UnknownLock                    Lock type is unknown, or error.
          \value DeviceUnlocked                 Device is unlocked.
          \value DeviceLocked                   Device is locked.
          \value TouchAndKeyboardLocked         Touch and/or keyboard lock.

          */


        /*!
          \fn void QSystemDeviceInfo::bluetoothStateChanged(bool on)

          This signal is emitted whenever bluetooth state changes, specified by \a on.
        */

/*!
  \fn void QSystemDeviceInfo::wirelessKeyboardConnected(bool connected)

  This signal is emitted whenever a wireless keyboard is connected, specified by \a connected
*/
/*!
  \fn void QSystemDeviceInfo::keyboardFlip(bool open)

  This signal is emitted whenever a phone flips open, specified by \a open.
*/

/*!
    \fn void QSystemDeviceInfo::lockChanged(QSystemDeviceInfo::LockType type, bool on)

     This signal is emitted whenever the lock state changes, with LockType \a type, and \a on, locked or unlocked.
*/
/*!
    \fn void QSystemDeviceInfo::deviceLocked(bool isLocked)

     This signal is emitted whenever the device lock state changes, indicated by \a on, locked or unlocked.
*/

/*!
   \brief The QSystemDeviceInfo class provides access to device information from the system.

   \fn QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)

   Constructs a QSystemDeviceInfo with the given \a parent.
   */

QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)
    : QObject(parent), d(deviceInfoPrivate())
{
    qRegisterMetaType<QSystemDeviceInfo::BatteryStatus>("QSystemDeviceInfo::BatteryStatus");
    qRegisterMetaType<QSystemDeviceInfo::PowerState>("QSystemDeviceInfo::PowerState");
    qRegisterMetaType<QSystemDeviceInfo::SimStatus>("QSystemDeviceInfo::SimStatus");
    qRegisterMetaType<QSystemDeviceInfo::Profile>("QSystemDeviceInfo::Profile");
    qRegisterMetaType<QSystemDeviceInfo::InputMethodFlags>("QSystemDeviceInfo::InputMethodFlags");
    qRegisterMetaType<QSystemDeviceInfo::LockType>("QSystemDeviceInfo::LockType");
    qRegisterMetaType<QSystemDeviceInfo::keypadType>("QSystemDeviceInfo::keypadType");
}

/*!
  Destroys the QSystemDeviceInfo object.
 */
QSystemDeviceInfo::~QSystemDeviceInfo()
{
}


/*!
    \internal

    This function is called when the client connects to signals.

    \sa connectNotify()
*/

void QSystemDeviceInfo::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryLevelChanged(int))))) {
        connect(d,SIGNAL(batteryLevelChanged(int)),
                this,SIGNAL(batteryLevelChanged(int)),Qt::UniqueConnection);
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryStatusChanged(QSystemDeviceInfo::BatteryStatus))))) {
        connect(d,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),Qt::UniqueConnection);
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            bluetoothStateChanged(bool))))) {
        connect(d,SIGNAL(bluetoothStateChanged(bool)),
                this,SIGNAL(bluetoothStateChanged(bool)),Qt::UniqueConnection);
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentProfileChanged(QSystemDeviceInfo::Profile))))) {
        connect(d,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                this,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),Qt::UniqueConnection);
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            powerStateChanged(QSystemDeviceInfo::PowerState))))) {
        connect(d,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                this,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),Qt::UniqueConnection);
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            wirelessKeyboardConnected(bool))))) {
        connect(d,SIGNAL(wirelessKeyboardConnected(bool)),
                this,SIGNAL(wirelessKeyboardConnected(bool)),Qt::UniqueConnection);
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            keyboardFlip(bool))))) {
        connect(d,SIGNAL(keyboardFlip(bool)),
                this,SIGNAL(keyboardFlip(bool)),Qt::UniqueConnection);
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            deviceLocked(bool))))) {
        connect(d,SIGNAL(deviceLocked(bool)),
                this,SIGNAL(deviceLocked(bool)),Qt::UniqueConnection);
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            lockStatusChanged(QSystemDeviceInfo::LockType))))) {
        connect(d,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),
                this,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),Qt::UniqueConnection);
    }
}

/*!
    \internal

    This function is called when the client disconnects from the signals.

    \sa connectNotify()
*/
void QSystemDeviceInfo::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryLevelChanged(int))))) {
        disconnect(d,SIGNAL(batteryLevelChanged(int)),
                this,SIGNAL(batteryLevelChanged(int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryStatusChanged(QSystemDeviceInfo::BatteryStatus))))) {
        disconnect(d,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            bluetoothStateChanged(bool))))) {
        disconnect(d,SIGNAL(bluetoothStateChanged(bool)),
                this,SIGNAL(bluetoothStateChanged(bool)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentProfileChanged(QSystemDeviceInfo::Profile))))) {
        disconnect(d,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                this,SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            powerStateChanged(QSystemDeviceInfo::PowerState))))) {
        disconnect(d,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                this,SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            wirelessKeyboardConnected(bool))))) {
        disconnect(d,SIGNAL(wirelessKeyboardConnected(bool)),
                this,SIGNAL(wirelessKeyboardConnected(bool)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            wirelessKeyboardConnected(bool))))) {
        disconnect(d,SIGNAL(wirelessKeyboardConnected(bool)),
                this,SIGNAL(wirelessKeyboardConnected(bool)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            keyboardFlip(bool))))) {
        disconnect(d,SIGNAL(keyboardFlip(bool)),
                this,SIGNAL(keyboardFlip(bool)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            deviceLocked(bool))))) {
        disconnect(d,SIGNAL(deviceLocked(bool)),
                this,SIGNAL(deviceLocked(bool)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            lockStatusChanged(QSystemDeviceInfo::LockType))))) {
        disconnect(d,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),
                this,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)));
    }
}


/*!
  \property QSystemDeviceInfo::inputMethodType
  \brief The supported inputmethods.

    Returns the QSystemDeviceInfo::InputMethodFlags InputMethodType that the system uses.
*/
QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfo::inputMethodType()
{
 return deviceInfoPrivate()->inputMethodType();
}

/*!
  \property QSystemDeviceInfo::imei
  \brief The IMEI.

    Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imei()
{
    return deviceInfoPrivate()->imei();
}

/*!
  \property QSystemDeviceInfo::imsi
  \brief The IMSI.

    Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none.
*/
QString QSystemDeviceInfo::imsi()
{
    return deviceInfoPrivate()->imsi();
}

/*!
  \property QSystemDeviceInfo::manufacturer
  \brief The manufacture's name.

    Returns the name of the manufacturer of this device. In the case of desktops, the name of the vendor
    of the motherboard.
*/
QString QSystemDeviceInfo::manufacturer()
{
    return deviceInfoPrivate()->manufacturer();
}

/*!
  \property QSystemDeviceInfo::model
  \brief The model name.

    Returns the model information of the device. In the case of desktops where no
    model information is present, the CPU architect, such as i686, and machine type, such as Server,
    Desktop or Laptop.
*/
QString QSystemDeviceInfo::model()
{
    return deviceInfoPrivate()->model();
}

/*!
  \property QSystemDeviceInfo::productName
  \brief The product name.

    Returns the product name of the device. In the case where no product information is available, an empty string will be returned.

*/
QString QSystemDeviceInfo::productName()
{
    return deviceInfoPrivate()->productName();
}
/*!
  \property  QSystemDeviceInfo::batteryLevel
  \brief The battery level.

    Returns the battery charge level as percentage 1 - 100 scale.
*/
int QSystemDeviceInfo::batteryLevel() const
{
    return deviceInfoPrivate()->batteryLevel();
}

  /*!
    \property QSystemDeviceInfo::batteryStatus
    \brief The battery status.

    Returns the battery charge status.
*/
QSystemDeviceInfo::BatteryStatus QSystemDeviceInfo::batteryStatus()
{
   int level = batteryLevel();
    if(level < 4) {
        return QSystemDeviceInfo::BatteryCritical;
    }   else if(level < 11) {
        return QSystemDeviceInfo::BatteryVeryLow;
    }  else if(level < 41) {
        return QSystemDeviceInfo::BatteryLow;
    }   else if(level > 40) {
        return QSystemDeviceInfo::BatteryNormal;
    }

    return QSystemDeviceInfo::NoBatteryLevel;
}

/*!
  \property QSystemDeviceInfo::simStatus
  \brief the status of the sim card.
  Returns the QSystemDeviceInfo::simStatus status of SIM card.
*/
QSystemDeviceInfo::SimStatus QSystemDeviceInfo::simStatus()
{
    return deviceInfoPrivate()->simStatus();
}
/*!
  \property QSystemDeviceInfo::isDeviceLocked
  \brief Device lock.

  Returns true if the device is locked, otherwise false.
*/
bool QSystemDeviceInfo::isDeviceLocked()
{
    return deviceInfoPrivate()->isDeviceLocked();
}

/*!
  \property QSystemDeviceInfo::currentProfile
  \brief the device profile
  Gets the current QSystemDeviceInfo::currentProfile device profile.
*/
QSystemDeviceInfo::Profile QSystemDeviceInfo::currentProfile()
{
    return deviceInfoPrivate()->currentProfile();
}

/*!
  \property QSystemDeviceInfo::currentPowerState
  \brief the power state.

  Gets the current QSystemDeviceInfo::currentPowerState state.
*/
QSystemDeviceInfo::PowerState QSystemDeviceInfo::currentPowerState()
{
    return deviceInfoPrivate()->currentPowerState();
}

/*!
 \property QSystemDeviceInfo::currentBluetoothPowerState
 \brief bluetooth power state.

 Gets the current bluetooth power state.
 */
bool QSystemDeviceInfo::currentBluetoothPowerState()
{
    return deviceInfoPrivate()->currentBluetoothPowerState();
}

/*!
  \property QSystemDeviceInfo::keyboardType
  \brief The Keyboard Type

  Returns the type of keyboards found.
  */
QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfo::keyboardType()
{
    return deviceInfoPrivate()->keyboardType();
}

/*!
  \property QSystemDeviceInfo::isWirelessKeyboardConnected
  \brief wireless keyboard connected

  Returns true if a wireless keyboard is connected, otherwise false;
  */
bool QSystemDeviceInfo::isWirelessKeyboardConnected()
{
    return deviceInfoPrivate()->isWirelessKeyboardConnected();
}

/*!
  \property QSystemDeviceInfo::isKeyboardFlipOpen
  \brief Flip keyboard open.

  Returns true if the flip keyboard is open, otherwise false;
  */
bool QSystemDeviceInfo::isKeyboardFlipOpen()
{
    return deviceInfoPrivate()->isKeyboardFlipOpen();
}

/*!
  \property QSystemDeviceInfo::keypadLightOn
  \brief Keypad light on.

  Returns true if the key pad, indicated by \a type, light is on, otherwise false;
  */
bool QSystemDeviceInfo::keypadLightOn(QSystemDeviceInfo::keypadType type)
{
    return deviceInfoPrivate()->keypadLightOn(type);
}


/*!
  \property QSystemDeviceInfo::hostId
  \brief unique host id.

  Returns a unique identifier for the machine.

  Depending on security enforcement on platform, this may return a non unique number, or 0.

  */
QUuid QSystemDeviceInfo::hostId()
{
    return deviceInfoPrivate()->hostId();
}

/*!
  \property QSystemDeviceInfo::lockStatus
 \brief Type of lock.

 Returns the QSystemDeviceInfo::DeviceType type of lock the device might be in.
 */
QSystemDeviceInfo::LockType QSystemDeviceInfo::lockStatus()
{
    return deviceInfoPrivate()->lockStatus();
}

#include "moc_qsystemdeviceinfo.cpp"

QTM_END_NAMESPACE

