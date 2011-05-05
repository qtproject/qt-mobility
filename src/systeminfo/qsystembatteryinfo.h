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


#ifndef QSYSTEMBATTERYINFO_H
#define QSYSTEMBATTERYINFO_H

#include <QObject>
#include "qmobilityglobal.h"

QT_BEGIN_HEADER
QTM_BEGIN_NAMESPACE

class QSystemBatteryInfoPrivate;

class Q_SYSINFO_EXPORT QSystemBatteryInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int nominalCapacity READ nominalCapacity NOTIFY nominalCapacityChanged)
    Q_PROPERTY(int remainingCapacityPercent READ remainingCapacityPercent NOTIFY remainingCapacityPercentChanged)
    Q_PROPERTY(int remainingCapacity READ remainingCapacity NOTIFY remainingCapacityChanged)
    Q_PROPERTY(int voltage READ voltage CONSTANT)
    Q_PROPERTY(int remainingChargingTime READ remainingChargingTime NOTIFY remainingChargingTimeChanged)

    Q_PROPERTY(int currentFlow READ currentFlow  NOTIFY currentFlowChanged)
    Q_PROPERTY(int remainingCapacityBars READ remainingCapacityBars NOTIFY remainingCapacityBarsChanged)
    Q_PROPERTY(int maxBars READ maxBars CONSTANT)

    Q_PROPERTY(ChargerType chargerType READ chargerType NOTIFY chargerTypeChanged)
    Q_PROPERTY(ChargingState chargingState READ chargingState NOTIFY chargingStateChanged)
    Q_PROPERTY(EnergyUnit energyMeasurementUnit READ energyMeasurementUnit CONSTANT)
    Q_PROPERTY(BatteryStatus batteryStatus READ batteryStatus NOTIFY batteryStatusChanged)

    Q_ENUMS(BatteryStatus)
    Q_ENUMS(ChargerType)
    Q_ENUMS(ChargingState)
    Q_ENUMS(EnergyUnit)


public:
    explicit QSystemBatteryInfo(QObject *parent = 0);
    ~QSystemBatteryInfo();

    enum BatteryStatus {
        BatteryUnknown = -1,
        BatteryEmpty,
        BatteryCritical,
        BatteryVeryLow,
        BatteryLow,
        BatteryOk,
        BatteryFull
    };

    enum ChargingState {
        ChargingError = -1,
        NotCharging,
        Charging
    };

    enum ChargerType {
        UnknownCharger = -1,
        NoCharger,
        WallCharger,
        USBCharger,
        USB_500mACharger,
        USB_100mACharger,
        VariableCurrentCharger
    };

    enum EnergyUnit {
        UnitUnknown = -1,
        UnitmAh,
        UnitmWh
    };

    QSystemBatteryInfo::ChargerType chargerType() const;
    QSystemBatteryInfo::ChargingState chargingState() const;

    int nominalCapacity() const;
    int remainingCapacityPercent() const;
    int remainingCapacity() const;

    int voltage() const;
    int remainingChargingTime() const;
    int currentFlow() const;

    int remainingCapacityBars() const;
    int maxBars() const;
    QSystemBatteryInfo::BatteryStatus batteryStatus() const;
    QSystemBatteryInfo::EnergyUnit energyMeasurementUnit() const;

Q_SIGNALS:
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);

    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void nominalCapacityChanged(int cap);
    void remainingCapacityPercentChanged(int level);
    void remainingCapacityChanged(int level);

    void currentFlowChanged(int level);
    void remainingCapacityBarsChanged(int bars);
    void remainingChargingTimeChanged(int time);

private:
    QSystemBatteryInfoPrivate *d;

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

};

QTM_END_NAMESPACE
QT_END_HEADER
#endif // QSYSTEMBATTERYINFO_H
