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
    out << s.batteryLevel << s.deviceLocked;
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemDeviceInfoData &s)
{
    in >> s.imei >> s.imsi >> s.manufacturer >> s.model >> s.productName;
    qint32 inputMethod, simStatus, profile, powerState;
    in >> inputMethod >> simStatus >> profile >> powerState;
    s.inputMethodType = static_cast<QSystemDeviceInfo::InputMethodFlags>(inputMethod);
    s.simStatus = static_cast<QSystemDeviceInfo::SimStatus>(simStatus);
    s.currentProfile = static_cast<QSystemDeviceInfo::Profile>(profile);
    s.currentPowerState = static_cast<QSystemDeviceInfo::PowerState>(powerState);
    in >> s.batteryLevel >> s.deviceLocked;
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
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemStorageInfoData::DriveInfo &s)
{
    qint32 type;
    in >> type;
    s.type = static_cast<QSystemStorageInfo::DriveType>(type);
    in >> s.availableSpace >> s.totalSpace;
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
    return out;
}
QDataStream &operator>>(QDataStream &in, QSystemDisplayInfoData &s)
{
    qint32 depth, brightness;
    in >> depth >> brightness;
    s.colorDepth = depth;
    s.displayBrightness = brightness;
    return in;
}

QTM_END_NAMESPACE
