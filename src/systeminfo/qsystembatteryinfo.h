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
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus status);

    void chargingStateChanged(QSystemBatteryInfo::ChargingState state);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType type);

    void nominalCapacityChanged(int capacity);
    void remainingCapacityPercentChanged(int capacity);
    void remainingCapacityChanged(int capacity);

    void currentFlowChanged(int flow);
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
