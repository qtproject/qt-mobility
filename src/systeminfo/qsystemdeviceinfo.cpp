/****************************************************************************
**
** Copyright (C) 2009-2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#ifndef Q_OS_SYMBIAN
Q_GLOBAL_STATIC(QSystemDeviceInfoPrivate, deviceInfoPrivateSingleton)
#endif

#ifdef QT_SIMULATOR
QSystemDeviceInfoPrivate *getSystemDeviceInfoPrivate() { return deviceInfoPrivateSingleton(); }
#endif // QT_SIMULATOR

/*!
    \class QSystemDeviceInfo
    \ingroup systeminfo
    \inmodule QtSystemInfo
    \brief The QSystemDeviceInfo class provides access to device information from the system.
    \since 1.0

    \reentrant

    \note All functions in this class are reentrant.

    \warning On Symbian this class does not support QObject::moveToThread().
*/

/*!
    \class QSystemDeviceInfo::ProfileDetails
    \ingroup systeminfo
    \inmodule QtSystemInfo
    \brief The ProfileDetails class provides access to details of the currently active phone profile.
    \since 1.2

    \reentrant

    \note All functions in this class are reentrant.
*/

/*!
    \fn void QSystemDeviceInfo::batteryLevelChanged(int level)

    This signal is emitted when battery level has changed to \a level.
    \since 1.1
*/

/*!
    \fn void QSystemDeviceInfo::batteryStatusChanged(QSystemDeviceInfo::BatteryStatus status)

    This signal is emitted when battery status has changed to \a status.
    \since 1.1
*/

/*!
    \fn void QSystemDeviceInfo::powerStateChanged(QSystemDeviceInfo::PowerState state)

    This signal is emitted when the power state has changed to \a state, such as when a phone gets
    plugged in to the wall
    \since 1.1
*/

/*!
    \fn void QSystemDeviceInfo::thermalStateChanged(QSystemDeviceInfo::ThermalState state)

    This signal is emitted when the thermal state has changed to \a state.
    \since 1.2
*/

/*!
    \fn  void QSystemDeviceInfo::currentProfileChanged(QSystemDeviceInfo::Profile profile)

    This signal is emitted whenever the users active profile changes to \a profile.
    \since 1.1
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
    \enum QSystemDeviceInfo::ThermalState
    This enum describes the thermal state:

    \value UnknownThermal                 UnKnown State.
    \value NormalThermal                  Normal State.
    \value WarningThermal                 Warning State.
    \value AlertThermal                   Alert State.
    \value ErrorThermal                   Thermal Error.
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
    \value FlipKeyboard                Keybord or keypad the slides or flips out.
*/

/*!
    \enum QSystemDeviceInfo::KeypadType
    This enum describe the type of keypad/keyboard.

    \value PrimaryKeypad                 Primary keypad or keyboard used.
    \value SecondaryKeypad               Secondary keypad or keyboard used.
*/

/*!
    \enum QSystemDeviceInfo::LockType
    This enum describes the type of lock.

    \value UnknownLock                    Lock type is unknown, or error.
    \value PinLocked                      Device is PIN or password locked.
    \value TouchAndKeyboardLocked         Touch and/or keyboard is locked.
*/

/*!
    \fn void QSystemDeviceInfo::bluetoothStateChanged(bool on)

    This signal is emitted whenever bluetooth state changes to \a on.
    \since 1.0
*/

/*!
    \fn void QSystemDeviceInfo::wirelessKeyboardConnected(bool connected)

    This signal is emitted whenever a wireless keyboard is connected, specified by \a connected
    \since 1.2
*/

/*!
    \fn void QSystemDeviceInfo::keyboardFlipped(bool open)

    This signal is emitted whenever a phone flips open, specified by \a open.
    \since 1.2
*/

/*!
    \fn void QSystemDeviceInfo::lockStatusChanged(QSystemDeviceInfo::LockTypeFlags type)

    This signal is emitted whenever the lock state changes, with LockType \a type.
    \since 1.2
*/

/*!
    \fn void QSystemDeviceInfo::deviceLocked(bool isLocked)

    This signal is emitted whenever the device lock state changes to \a isLocked.
   \since 1.2
*/

/*!
    \brief The QSystemDeviceInfo class provides access to device information from the system.

    \fn QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)

    Constructs a QSystemDeviceInfo with the given \a parent.
*/
QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent)
    : QObject(parent)
{
#ifdef Q_OS_SYMBIAN
    d = new QSystemDeviceInfoPrivate();
#else
    d = deviceInfoPrivateSingleton();
#endif
    qRegisterMetaType<QSystemDeviceInfo::BatteryStatus>("QSystemDeviceInfo::BatteryStatus");
    qRegisterMetaType<QSystemDeviceInfo::PowerState>("QSystemDeviceInfo::PowerState");
    qRegisterMetaType<QSystemDeviceInfo::ThermalState>("QSystemDeviceInfo::ThermalState");
    qRegisterMetaType<QSystemDeviceInfo::SimStatus>("QSystemDeviceInfo::SimStatus");
    qRegisterMetaType<QSystemDeviceInfo::Profile>("QSystemDeviceInfo::Profile");
    qRegisterMetaType<QSystemDeviceInfo::InputMethodFlags>("QSystemDeviceInfo::InputMethodFlags");
    qRegisterMetaType<QSystemDeviceInfo::LockTypeFlags>("QSystemDeviceInfo::LockTypeFlags");
    qRegisterMetaType<QSystemDeviceInfo::KeypadType>("QSystemDeviceInfo::KeypadType");
    qRegisterMetaType<QSystemDeviceInfo::KeyboardType>("QSystemDeviceInfo::KeyboardType");
}

/*!
    Destroys the QSystemDeviceInfo object.
 */
QSystemDeviceInfo::~QSystemDeviceInfo()
{
#ifdef Q_OS_SYMBIAN
    delete d;
#endif
}

/*!
    \internal
*/
void QSystemDeviceInfo::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(batteryLevelChanged(int))) {
        connect(d, SIGNAL(batteryLevelChanged(int)),
                this, SIGNAL(batteryLevelChanged(int)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus))) {
        connect(d, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
                this, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(bluetoothStateChanged(bool))) {
        connect(d, SIGNAL(bluetoothStateChanged(bool)),
                this, SIGNAL(bluetoothStateChanged(bool)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile))) {
        connect(d, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                this, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState))) {
        connect(d, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                this, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(thermalStateChanged(QSystemDeviceInfo::ThermalState))) {
        connect(d, SIGNAL(thermalStateChanged(QSystemDeviceInfo::ThermalState)),
                this, SIGNAL(thermalStateChanged(QSystemDeviceInfo::ThermalState)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(wirelessKeyboardConnected(bool))) {
        connect(d, SIGNAL(wirelessKeyboardConnected(bool)),
                this, SIGNAL(wirelessKeyboardConnected(bool)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(keyboardFlipped(bool))) {
        connect(d, SIGNAL(keyboardFlipped(bool)),
                this, SIGNAL(keyboardFlipped(bool)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(deviceLocked(bool))) {
        connect(d, SIGNAL(deviceLocked(bool)),
                this, SIGNAL(deviceLocked(bool)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags))) {
        connect(d, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)),
                this, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)),
                Qt::UniqueConnection);
    }
}

/*!
    \internal
*/
void QSystemDeviceInfo::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(batteryLevelChanged(int))) {
        disconnect(d, SIGNAL(batteryLevelChanged(int)),
                   this, SIGNAL(batteryLevelChanged(int)));
    } else if (QLatin1String(signal) == SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus))) {
        disconnect(d, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
                   this, SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
    } else if (QLatin1String(signal) == SIGNAL(bluetoothStateChanged(bool))) {
        disconnect(d, SIGNAL(bluetoothStateChanged(bool)),
                   this, SIGNAL(bluetoothStateChanged(bool)));
    } else if (QLatin1String(signal) == SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile))) {
        disconnect(d, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
                   this, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)));
    } else if (QLatin1String(signal) == SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState))) {
        disconnect(d, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                   this, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)));
    } else if (QLatin1String(signal) == SIGNAL(thermalStateChanged(QSystemDeviceInfo::ThermalState))) {
        disconnect(d, SIGNAL(thermalStateChanged(QSystemDeviceInfo::ThermalState)),
                   this, SIGNAL(thermalStateChanged(QSystemDeviceInfo::ThermalState)));
    } else if (QLatin1String(signal) == SIGNAL(wirelessKeyboardConnected(bool))) {
        disconnect(d, SIGNAL(wirelessKeyboardConnected(bool)),
                   this, SIGNAL(wirelessKeyboardConnected(bool)));
    } else if (QLatin1String(signal) == SIGNAL(wirelessKeyboardConnected(bool))) {
        disconnect(d, SIGNAL(wirelessKeyboardConnected(bool)),
                   this, SIGNAL(wirelessKeyboardConnected(bool)));
    } else if (QLatin1String(signal) == SIGNAL(keyboardFlipped(bool))) {
        disconnect(d, SIGNAL(keyboardFlipped(bool)),
                   this, SIGNAL(keyboardFlipped(bool)));
    } else if (QLatin1String(signal) == SIGNAL(deviceLocked(bool))) {
        disconnect(d, SIGNAL(deviceLocked(bool)),
                   this, SIGNAL(deviceLocked(bool)));
    } else if (QLatin1String(signal) == SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags))) {
        disconnect(d, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)),
                   this, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockTypeFlags)));
    }
}


/*!
    \property QSystemDeviceInfo::inputMethodType
    \brief The supported inputmethods.

    Returns the QSystemDeviceInfo::InputMethodFlags InputMethodType that the system uses.
    \since 1.0
*/
QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfo::inputMethodType()
{
    return d->inputMethodType();
}

/*!
    \property QSystemDeviceInfo::imei
    \brief The IMEI.

    Returns the International Mobile Equipment Identity (IMEI), or a null QString in the case of none.
    \since 1.0
*/
QString QSystemDeviceInfo::imei()
{
    return d->imei();
}

/*!
    \property QSystemDeviceInfo::imsi
    \brief The IMSI.

    Returns the International Mobile Subscriber Identity (IMSI), or a null QString in the case of none.
    \since 1.0
*/
QString QSystemDeviceInfo::imsi()
{
    return d->imsi();
}

/*!
    \property QSystemDeviceInfo::manufacturer
    \brief The manufacture's name.

    Returns the name of the manufacturer of this device. In the case of desktops, the name of the vendor
    of the motherboard.
    \since 1.0
*/
QString QSystemDeviceInfo::manufacturer()
{
    return d->manufacturer();
}

/*!
    \property QSystemDeviceInfo::model
    \brief The model name.

    Returns the public-known model information of the device, e.g. N8, E7, etc. In the case where no
    model information is present, the CPU architect, such as i686, or machine type, such as Server,
    Desktop or Laptop.
    \since 1.0
*/
QString QSystemDeviceInfo::model()
{
    return d->model();
}

/*!
    \property QSystemDeviceInfo::productName
    \brief The product name.

    Returns the internal product name of the device, e.g. RM-774. In the case where no product name
    is available, an empty string is returned.
    \since 1.0
*/
QString QSystemDeviceInfo::productName()
{
    return d->productName();
}
/*!
    \property QSystemDeviceInfo::batteryLevel
    \brief The battery level.

    Returns the battery charge level as percentage 1 - 100 scale.
    \since 1.0
*/
int QSystemDeviceInfo::batteryLevel() const
{
    return d->batteryLevel();
}

/*!
    \property QSystemDeviceInfo::batteryStatus
    \brief The battery status.

    Returns the battery charge status.
    \since 1.0
*/
QSystemDeviceInfo::BatteryStatus QSystemDeviceInfo::batteryStatus()
{
    int level = batteryLevel();
    if (level < 4)
        return QSystemDeviceInfo::BatteryCritical;
    else if(level < 11)
        return QSystemDeviceInfo::BatteryVeryLow;
    else if(level < 41)
        return QSystemDeviceInfo::BatteryLow;
    else if(level > 40)
        return QSystemDeviceInfo::BatteryNormal;

    return QSystemDeviceInfo::NoBatteryLevel;
}

/*!
    \property QSystemDeviceInfo::simStatus
    \brief the status of the sim card.

    Returns the QSystemDeviceInfo::simStatus status of SIM card.
    \since 1.0
*/
QSystemDeviceInfo::SimStatus QSystemDeviceInfo::simStatus()
{
    return d->simStatus();
}

/*!
    \property QSystemDeviceInfo::isDeviceLocked
    \brief Device lock.

    Returns true if the device is locked, otherwise false.
    \since 1.0
*/
bool QSystemDeviceInfo::isDeviceLocked()
{
    return d->isDeviceLocked();
}

/*!
    \property QSystemDeviceInfo::currentProfile
    \brief the device profile

    Gets the current QSystemDeviceInfo::currentProfile device profile.
    \since 1.1
*/
QSystemDeviceInfo::Profile QSystemDeviceInfo::currentProfile()
{
    return d->currentProfile();
}

/*!
    \property QSystemDeviceInfo::currentPowerState
    \brief the power state.

    Gets the current QSystemDeviceInfo::currentPowerState state.
    \since 1.2
*/
QSystemDeviceInfo::PowerState QSystemDeviceInfo::currentPowerState()
{
    return d->currentPowerState();
}

/*!
    \property QSystemDeviceInfo::currentThermalState
    \brief the thermal state.

    Gets the current QSystemDeviceInfo::currentThermalState state.
    \since 1.2
*/
QSystemDeviceInfo::ThermalState QSystemDeviceInfo::currentThermalState()
{
    return d->currentThermalState();
}

/*!
    \property QSystemDeviceInfo::currentBluetoothPowerState
    \brief bluetooth power state.

    Gets the current bluetooth power state.
    \since 1.1
*/
bool QSystemDeviceInfo::currentBluetoothPowerState()
{
    return d->currentBluetoothPowerState();
}

/*!
    \property QSystemDeviceInfo::keyboardTypes
    \brief The Keyboard Type

    Returns the type of keyboards found.
    \since 1.2
*/
QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfo::keyboardTypes()
{
    return d->keyboardTypes();
}

/*!
    \property QSystemDeviceInfo::isWirelessKeyboardConnected
    \brief wireless keyboard connected

    Returns true if a wireless keyboard is connected, otherwise false;
    \since 1.2
*/
bool QSystemDeviceInfo::isWirelessKeyboardConnected()
{
    return d->isWirelessKeyboardConnected();
}

/*!
    \property QSystemDeviceInfo::isKeyboardFlippedOpen
    \brief Flip keyboard open.

    Returns true if the flip keyboard is open, otherwise false;
    \since 1.2
*/
bool QSystemDeviceInfo::isKeyboardFlippedOpen()
{
    return d->isKeyboardFlippedOpen();
}

/*!
    Returns true if the key pad, indicated by \a type, light is on, otherwise false;
    \since 1.2
*/
bool QSystemDeviceInfo::keypadLightOn(QSystemDeviceInfo::KeypadType type)
{
    return d->keypadLightOn(type);
}

/*!
    Returns a unique identifier for the machine.

    Depending on security enforcement on platform, this may return a non unique number, or 0.
    This will be a 160 bit hex QByteArray unique ID constant to this device.
    \since 1.2
*/
QByteArray QSystemDeviceInfo::uniqueDeviceID()
{
    return d->uniqueDeviceID();
}

/*!
    \property QSystemDeviceInfo::lockStatus
    \brief Type of lock.

    Returns the QSystemDeviceInfo::LockTypeFlags type of lock state the device might be in.
    The LockType must be currently active not just enabled.
    \since 1.2
*/
QSystemDeviceInfo::LockTypeFlags QSystemDeviceInfo::lockStatus()
{
    return d->lockStatus();
}

/*!
    Returns a QSystemDeviceInfo::ProfileDetails for the currently active profile.
    \since 1.2
*/
QSystemDeviceInfo::ProfileDetails QSystemDeviceInfo::activeProfileDetails()
{
    return d->currentProfileDetails;
}


/*!
    Constructs a null ProfileDetails.
*/
QSystemDeviceInfo::ProfileDetails::ProfileDetails()
{
}

/*!
    Constructs the ProfileDetails that is a copy of \a other.
*/
QSystemDeviceInfo::ProfileDetails::ProfileDetails(const ProfileDetails& other)
{
    Q_UNUSED(other)
}

/*!
    Assigns \a other to this ProfileDetails and returns a reference to it.
*/
QSystemDeviceInfo::ProfileDetails &QSystemDeviceInfo::ProfileDetails::operator=(const ProfileDetails& other)
{
    Q_UNUSED(other)
    return *this;
}
/*!
    Destroys the ProfileDetails.
*/
QSystemDeviceInfo::ProfileDetails::~ProfileDetails()
{

}

/*!
    Returns the active profile's message ringtone volume. From 0 to 100.
*/
int QSystemDeviceInfo::ProfileDetails::messageRingtoneVolume() const
{
#ifdef Q_OS_SYMBIAN
     QSystemDeviceInfoPrivate* deviceInfo = QSystemDeviceInfoPrivate::deviceinfoPrivateInstance();
     int messageRingtoneVolume = deviceInfo->messageRingtoneVolume();
     delete deviceInfo;
     return messageRingtoneVolume;
#else
    return deviceInfoPrivateSingleton()->messageRingtoneVolume();
#endif
}

/*!
    Returns the active profile's voice ringtone volume. From 0 to 100.

*/
int QSystemDeviceInfo::ProfileDetails::voiceRingtoneVolume() const
{
#ifdef Q_OS_SYMBIAN
     QSystemDeviceInfoPrivate* deviceInfo = QSystemDeviceInfoPrivate::deviceinfoPrivateInstance();
     int voiceRingtoneVolume = deviceInfo->voiceRingtoneVolume();
     delete deviceInfo;
     return voiceRingtoneVolume;
#else
    return deviceInfoPrivateSingleton()->voiceRingtoneVolume();
#endif
}

/*!
    Returns the whether the active profile's vibration is active.

*/
bool QSystemDeviceInfo::ProfileDetails::vibrationActive() const
{
#ifdef Q_OS_SYMBIAN
     QSystemDeviceInfoPrivate* deviceInfo = QSystemDeviceInfoPrivate::deviceinfoPrivateInstance();
     int vibrationActive = deviceInfo->vibrationActive();
     delete deviceInfo;
     return vibrationActive;
#else
    return deviceInfoPrivateSingleton()->vibrationActive();
#endif
}

#include "moc_qsystemdeviceinfo.cpp"

QTM_END_NAMESPACE
