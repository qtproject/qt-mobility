/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSYSTEMINFOSTATE_SIMULATOR_P_H
#define QSYSTEMINFOSTATE_SIMULATOR_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//


#include "qsysteminfo.h"

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMetaType>

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

void qt_registerSystemInfoTypes();

struct QSystemInfoData
{
    QString currentLanguage;
    QStringList availableLanguages;
    QString currentCountryCode;

    QVector<QString> versions;
    QVector<bool> features;
};

struct QSystemNetworkInfoData
{
    struct NetworkInfo
    {
        QSystemNetworkInfo::NetworkStatus status;
        QString name;
        QString macAddress;
        qint32 signalStrength;
        QNetworkInterface interface;
    };

    QVector<NetworkInfo> networkInfo;

    int cellId;
    int locationAreaCode;

    QString currentMobileCountryCode;
    QString currentMobileNetworkCode;
    QString homeMobileCountryCode;
    QString homeMobileNetworkCode;
    QSystemNetworkInfo::NetworkMode currentMode;
    QSystemNetworkInfo::CellDataTechnology cellData;

};

struct QSystemDisplayInfoData
{
    int displayBrightness;
    int colorDepth;

    QSystemDisplayInfo::DisplayOrientation orientation;
    float contrast;
    QSystemDisplayInfo::BacklightState backlightStatus;

};

struct QSystemStorageInfoData
{
    struct DriveInfo
    {
        QSystemStorageInfo::DriveType type;
        qint64 totalSpace;
        qint64 availableSpace;
        QString uri;
        QSystemStorageInfo::StorageState state;
    };

    QHash<QString, DriveInfo> drives;
};

struct QSystemDeviceInfoData
{
    QString imei;
    QString imsi;
    QString manufacturer;
    QString model;
    QString productName;

    QSystemDeviceInfo::InputMethodFlags inputMethodType;
    QSystemDeviceInfo::SimStatus simStatus;
    QSystemDeviceInfo::Profile currentProfile;
    QSystemDeviceInfo::PowerState currentPowerState;
    QSystemDeviceInfo::ThermalState currentThermalState;

    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes;
    QSystemDeviceInfo::KeypadType keypadType;
    QSystemDeviceInfo::LockTypeFlags lockType;
    QSystemDeviceInfo::BatteryStatus batStatus;

    int batteryLevel;
    bool deviceLocked;
    bool currentBluetoothPower;

    bool wirelessConnected;
    bool keyboardFlipped;
    bool backLight;
    bool keypadLight;
    QByteArray uniqueDeviceId;

    int messageRingtoneVolume;
    int voiceRingtoneVolume;
    bool vibrationActive;

};

struct QSystemScreenSaverData
{
    int inhibitedCount;
};

struct QSystemBatteryInfoData
{
    QSystemBatteryInfo::BatteryStatus batteryStatus;
    QSystemBatteryInfo::ChargingState chargingState;
    QSystemBatteryInfo::ChargerType chargerType;
    QSystemBatteryInfo::EnergyUnit energyMeasurementUnit;

    int nominalCapacity;
    int remainingCapacityPercent;
    int remainingCapacity;

    int voltage;
    int remainingChargingTime;

    int currentFlow;
    int cumulativeCurrentFlow;
    int remainingCapacityBars;
    int maxBars;
};

QTM_END_NAMESPACE

// Can't use QTM_NAMESPACE here, since it won't be expanded.
Q_DECLARE_METATYPE(QtMobility::QSystemInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemNetworkInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemDisplayInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemStorageInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemDeviceInfoData)
Q_DECLARE_METATYPE(QtMobility::QSystemScreenSaverData)
Q_DECLARE_METATYPE(QtMobility::QSystemBatteryInfoData)

QT_END_HEADER

#endif // QSYSTEMINFOSTATE_SIMULATOR_P_H
