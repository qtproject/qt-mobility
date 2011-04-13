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

#ifndef QSYSTEMDEVICEINFO_H
#define QSYSTEMDEVICEINFO_H

#include <QObject>
#include <QExplicitlySharedDataPointer>

#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemDeviceInfoPrivate;

class  Q_SYSINFO_EXPORT QSystemDeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Profile currentProfile READ currentProfile NOTIFY currentProfileChanged)
    Q_PROPERTY(PowerState currentPowerState READ currentPowerState NOTIFY powerStateChanged)
    Q_PROPERTY(ThermalState currentThermalState READ currentThermalState NOTIFY thermalStateChanged)
    Q_PROPERTY(SimStatus simStatus READ simStatus CONSTANT)
    Q_PROPERTY(BatteryStatus batteryStatus READ batteryStatus NOTIFY batteryStatusChanged)
    Q_PROPERTY(QSystemDeviceInfo::InputMethodFlags inputMethodType READ inputMethodType)

    Q_PROPERTY(QString imei READ imei CONSTANT)
    Q_PROPERTY(QString imsi READ imsi CONSTANT)
    Q_PROPERTY(QString manufacturer READ manufacturer CONSTANT)
    Q_PROPERTY(QString model READ model CONSTANT)
    Q_PROPERTY(QString productName READ productName CONSTANT)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(bool isDeviceLocked READ isDeviceLocked NOTIFY deviceLocked)
    Q_PROPERTY(bool currentBluetoothPowerState READ currentBluetoothPowerState NOTIFY bluetoothStateChanged)

    Q_PROPERTY(KeyboardTypeFlags keyboardTypes READ keyboardTypes)//1.2
    Q_PROPERTY(bool isWirelessKeyboardConnected READ isWirelessKeyboardConnected NOTIFY wirelessKeyboardConnected)//1.2
    Q_PROPERTY(bool isKeyboardFlippedOpen READ isKeyboardFlippedOpen NOTIFY keyboardFlipped)//1.2
    Q_PROPERTY(QSystemDeviceInfo::LockTypeFlags lockStatus READ lockStatus NOTIFY lockStatusChanged)

    Q_PROPERTY(QByteArray uniqueDeviceID READ uniqueDeviceID CONSTANT)

    Q_ENUMS(BatteryStatus)
    Q_ENUMS(PowerState)
    Q_ENUMS(ThermalState)
    Q_FLAGS(InputMethod InputMethodFlags)
    Q_ENUMS(SimStatus)
    Q_ENUMS(Profile)
    Q_ENUMS(LockType)
    Q_ENUMS(KeypadType)

    Q_FLAGS(KeyboardType KeyboardTypeFlags) //1.2
    Q_FLAGS(LockType LockTypeFlags) //1.2

public:

    explicit QSystemDeviceInfo(QObject *parent = 0);
    virtual ~QSystemDeviceInfo();

    enum BatteryStatus {
        NoBatteryLevel = 0,
        BatteryCritical,
        BatteryVeryLow,
        BatteryLow,
        BatteryNormal
    };

    enum PowerState {
        UnknownPower = 0,
        BatteryPower,
        WallPower,
        WallPowerChargingBattery
    };

    enum ThermalState {
        UnknownThermal = 0,
        NormalThermal,
        WarningThermal,
        AlertThermal,
        ErrorThermal
    };

    enum InputMethod {
        Keys = 0x0000001,
        Keypad = 0x0000002,
        Keyboard = 0x0000004,
        SingleTouch = 0x0000008,
        MultiTouch = 0x0000010,
        Mouse = 0x0000020
    };
    Q_DECLARE_FLAGS(InputMethodFlags, InputMethod)

    enum Profile {
        UnknownProfile = 0,
        SilentProfile,
        NormalProfile,
        LoudProfile,
        VibProfile,
        OfflineProfile,
        PowersaveProfile,
        CustomProfile,
        BeepProfile
    };


    enum SimStatus {
        SimNotAvailable = 0,
        SingleSimAvailable,
        DualSimAvailable,
        SimLocked
    };

    enum KeyboardType {
        UnknownKeyboard = 0,
        SoftwareKeyboard = 0x0000001,
        ITUKeypad = 0x0000002,
        HalfQwertyKeyboard = 0x0000004,
        FullQwertyKeyboard = 0x0000008,
        WirelessKeyboard = 0x0000010,
        FlipKeyboard = 0x0000020
      };//1.2
    Q_DECLARE_FLAGS(KeyboardTypeFlags, KeyboardType)//1.2

    enum KeypadType {
       PrimaryKeypad = 0,
       SecondaryKeypad
    }; //1.2
//    enum KeypadType {
//       PrimaryKeypad = 0x0000001,
//       SecondaryKeypad = 0x0000002
//    }; //1.2
//    Q_DECLARE_FLAGS(KeypadTypeFlags, KeypadType)//1.2


    enum LockType {
        UnknownLock = 0,
        PinLocked = 0x0000001,
        TouchAndKeyboardLocked = 0x0000002
    }; //1.2
    Q_DECLARE_FLAGS(LockTypeFlags, LockType)//1.2

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    QString imei();
    QString imsi();
    QString manufacturer();
    QString model();
    QString productName();
    int batteryLevel() const;
    QSystemDeviceInfo::BatteryStatus batteryStatus();

    bool isDeviceLocked();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::PowerState currentPowerState();
    QSystemDeviceInfo::ThermalState currentThermalState();

    bool currentBluetoothPowerState();

    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2
    bool isWirelessKeyboardConnected(); //1.2
    bool isKeyboardFlippedOpen();//1.2

    Q_INVOKABLE bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
//    QSystemDeviceInfo::KeypadTypeFlags keypadLightsOn(); //1.2
    QByteArray uniqueDeviceID(); //1.2
    QSystemDeviceInfo::LockTypeFlags lockStatus(); //1.2

    class  Q_SYSINFO_EXPORT ProfileDetails  {
    public:
        ProfileDetails();
        ProfileDetails(const ProfileDetails &);
        ProfileDetails &operator=(const ProfileDetails &);

        ~ProfileDetails();

        int messageRingtoneVolume() const;
        int voiceRingtoneVolume() const;
        bool vibrationActive() const;
    private:
         friend class QSystemDeviceInfo;
    };

    Q_INVOKABLE ProfileDetails activeProfileDetails();//1.2

Q_SIGNALS:
    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus);
    void powerStateChanged(QSystemDeviceInfo::PowerState powerState);
    void thermalStateChanged(QSystemDeviceInfo::ThermalState thermalState);
    void currentProfileChanged(QSystemDeviceInfo::Profile currentProfile);
    void bluetoothStateChanged(bool on);

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlipped(bool open);//1.2
    void deviceLocked(bool isLocked); // 1.2
    void lockStatusChanged(QSystemDeviceInfo::LockTypeFlags); //1.2


private:
    QSystemDeviceInfoPrivate *d;
    QSystemDeviceInfo::ProfileDetails currentProfileDetails;

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSystemDeviceInfo::InputMethodFlags )


QTM_END_NAMESPACE

QT_END_HEADER

#endif // QSYSTEMDEVICEINFO_H
