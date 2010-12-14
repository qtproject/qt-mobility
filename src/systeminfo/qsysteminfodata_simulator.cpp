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

#include "qsysteminfodata_simulator_p.h"
aqfgw
#include <QtCore/QDataStream>

QTM_BEGIN_NAMESPACE

void qt_registerSystemInfoTypes()
{
    qRegisterMetaTypeStreamOperators<QSystemInfoData>("QtMobility::QSystemInfoData");
    qRegisterMetaTypeStreamOperators<QSystemDeviceInfoData>("QtMobility::QSystemDeviceInfoData");
    qRegisterMetaTypeStreamOperators<QSystemStorageInfoData>("QtMobility::QSystemStorageInfoData");
    qRegisterMetaTypeStreamOperators<QSystemStorageInfoData::DriveInfo>("QtMobility::QSystemStorageInfoData::DriveInfo");
    qRegisterMetaTypeStreamOperators<QSystemNetworkInfoData>("QtMobility::QSystemNetworkInfoData");
    qRegisterMetaTypeStreamOperators<QSystemNetworkInfoData::NetworkInfo>("QtMobility::QSystemNetworkInfoData::NetworkInfo");
    qRegisterMetaTypeStreamOperators<QSystemDisplayInfoData>("QtMobility::QSystemDisplayInfoData");

    qRegisterMetaTypeStreamOperators<QSystemBatteryInfoData>("QtMobility::QSystemBatteryInfoData");

}

QDataStream &operator<<(QDataStream &out, const QSystemInfoData &s)
{
    out << s.currentLanguage << s.availableLanguages << s.currentCountryCode;
    out << s.versions << s.features;
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemInfoData &s)
{
    in >> s.currentLanguage >> s.availableLanguages >> s.currentCountryCode;
    in >> s.versions >> s.features;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemDeviceInfoData &s)
{
    out << s.imei << s.imsi << s.manufacturer << s.model << s.productName;
    out << static_cast<qint32>(s.inputMethodType);
    out << static_cast<qint32>(s.simStatus);
    out << static_cast<qint32>(s.currentProfile);
    out << static_cast<qint32>(s.currentPowerState);
    out << static_cast<qint32>(s.keyboardTypes);
    out << static_cast<qint32>(s.keypadType);

    out << static_cast<qint32>(s.lockType);
    out << static_cast<qint32>(s.batStatus);

    out << s.batteryLevel << s.deviceLocked;
    out << s.wirelessConnected << s.keyboardFlipped << s.backLight << s.keypadLight << s.uniqueDeviceId;
    out << s.messageRingtoneVolume << s.voiceRingtoneVolume << s.vibrationActive;
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemDeviceInfoData &s)
{
    in >> s.imei >> s.imsi >> s.manufacturer >> s.model >> s.productName;

    qint32 inputMethod, simStatus, profile, powerState, keyboardTypes, keypadType, lockType, batStatus;
    in >> inputMethod >> simStatus >> profile >> powerState;
    in >> keyboardTypes >> keypadType >> lockType >> batStatus;

    s.inputMethodType = static_cast<QSystemDeviceInfo::InputMethodFlags>(inputMethod);
    s.simStatus = static_cast<QSystemDeviceInfo::SimStatus>(simStatus);
    s.currentProfile = static_cast<QSystemDeviceInfo::Profile>(profile);
    s.currentPowerState = static_cast<QSystemDeviceInfo::PowerState>(powerState);

    in >> s.batteryLevel >> s.deviceLocked;
    in >> s.wirelessConnected >> s.keyboardFlipped >> s.backLight >> s.keypadLight;

    s.keyboardTypes = static_cast<QSystemDeviceInfo::KeyboardType>(keyboardTypes);
    s.keypadType = static_cast<QSystemDeviceInfo::KeypadType>(keypadType);
    s.lockType = static_cast<QSystemDeviceInfo::LockType>(lockType);
    s.batStatus = static_cast<QSystemDeviceInfo::BatteryStatus>(batStatus);

    in >> s.uniqueDeviceId;
    in >> s.messageRingtoneVolume >> s.voiceRingtoneVolume >> s.vibrationActive;

    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemStorageInfoData &s)
{
    out << s.drives;
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemStorageInfoData &s)
{
    in >> s.drives;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemStorageInfoData::DriveInfo &s)
{
    out << static_cast<qint32>(s.type) << s.availableSpace << s.totalSpace;
    out << static_cast<qint32>(s.state) << s.uri;
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemStorageInfoData::DriveInfo &s)
{
    qint32 type, state;
    in >> type >> state;
    s.type = static_cast<QSystemStorageInfo::DriveType>(type);
    in >> s.availableSpace >> s.totalSpace;
    s.state = static_cast<QSystemStorageInfo::StorageState>(state);
    in >> s.uri;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemNetworkInfoData &s)
{
    out << static_cast<qint32>(s.cellId) << static_cast<qint32>(s.locationAreaCode);
    out << s.currentMobileCountryCode << s.currentMobileNetworkCode;
    out << s.homeMobileCountryCode << s.homeMobileNetworkCode;
    qint32 currentMode = static_cast<qint32>(s.currentMode);
    out << currentMode;
    out << s.networkInfo;
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemNetworkInfoData &s)
{
    qint32 cellid, loc;
    in >> cellid >> loc;
    s.cellId = cellid;
    s.locationAreaCode = loc;
    in >> s.currentMobileCountryCode >> s.currentMobileNetworkCode;
    in >> s.homeMobileCountryCode >> s.homeMobileNetworkCode;
    qint32 currentMode;
    in >> currentMode;
    s.currentMode = static_cast<QSystemNetworkInfo::NetworkMode>(currentMode);
    in >> s.networkInfo;
    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemNetworkInfoData::NetworkInfo &s)
{
    out << s.name << s.macAddress;
    out << s.signalStrength << static_cast<qint32>(s.status);
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemNetworkInfoData::NetworkInfo &s)
{
    in >> s.name >> s.macAddress;
    in >> s.signalStrength;
    qint32 status;
    in >> status;
    s.status = static_cast<QSystemNetworkInfo::NetworkStatus>(status);
    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemDisplayInfoData &s)
{
    out << static_cast<qint32>(s.colorDepth) << static_cast<qint32>(s.displayBrightness);
    out << static_cast<qint32>(s.dpiHeight) << static_cast<qint32>(s.dpiWidth);
    out << static_cast<qint32>(s.physicalHeight) << static_cast<qint32>(s.physicalWidth);
    out << static_cast<qint32>(s.orientation);
    out << static_cast<qint32>(s.backlightStatus);
    return out;
}

QDataStream &operator>>(QDataStream &in, QSystemDisplayInfoData &s)
{
    qint32 depth, brightness, colorDepth, dpiHeight, dpiWidth, physicalHeight, physicalWidth, orientation,
            backlightStatus;
    in >> depth >> brightness;
    in >> colorDepth >> dpiHeight >> dpiWidth;
    in >> physicalHeight >> physicalWidth;
    in >> orientation;
    s.colorDepth = depth;
    s.displayBrightness = brightness;
    s.dpiHeight = dpiHeight;
    s.dpiWidth = dpiWidth;
    s.dpiHeight = dpiHeight;
    s.dpiHeight = physicalHeight;
    s.dpiHeight = physicalWidth;
    s.orientation = static_cast<QSystemDisplayInfo::DisplayOrientation>(orientation);
    s.backlightStatus = static_cast<QSystemDisplayInfo::BacklightState>(backlightStatus);

    return in;
}

QDataStream &operator<<(QDataStream &out, const QSystemBatteryInfoData &s)
{
    out << static_cast<qint32>(s.batteryStatus) << static_cast<qint32>(s.chargingState);
    out << static_cast<qint32>(s.chargerType);
    out << s.nominalCapacity << s.remainingCapacityPercent << s.remainingCapacity << s.voltage;
    out << s.remainingChargingTime << s.currentFlow << s.cumulativeCurrentFlow << s.remainingCapacityBars;
    out << s.maxBars;
    out << s.energyMeasurementUnit;
    return out;
}

QDataStream &operator>>(QDataStream &in, QSystemBatteryInfoData &s)
{
    qint32 batteryStatus, chargingState, chargerType, energyMeasurementUnit;
    in >> batteryStatus >> chargingState >> chargerType >> energyMeasurementUnit;

    s.batteryStatus = static_cast<QSystemBatteryInfo::BatteryStatus>(batteryStatus);
    s.chargingState = static_cast<QSystemBatteryInfo::ChargingState>(chargingState);
    s.chargerType = static_cast<QSystemBatteryInfo::ChargerType>(chargerType);

    s.energyMeasurementUnit = static_cast<QSystemBatteryInfo::EnergyUnit>(energyMeasurementUnit);

    in >> s.nominalCapacity >> s.remainingCapacityPercent >> s.remainingCapacity;
    in >> s.voltage >> s.remainingChargingTime >> s.currentFlow;
    in >> s.cumulativeCurrentFlow >> s.remainingCapacityBars >> s.maxBars;
    return in;
}

QTM_END_NAMESPACE
