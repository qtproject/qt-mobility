/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsystembatteryinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemBatteryInfoPrivate, batteryInfoPrivate)

#ifdef QT_SIMULATOR
QSystemBatteryInfoPrivate *getSystemBatteryInfoPrivate() { return batteryInfoPrivate(); }
#endif // QT_SIMULATOR

/*!
    \class QSystemBatteryInfo
    \ingroup systeminfo
    \inmodule QtSystemInfo
    \brief The QSystemBatteryInfo class provides access to battery and power information from the system.
    \since 1.2
*/

/*!
    \enum QSystemBatteryInfo::BatteryStatus
    This enum describes the status of the main battery.

    \value BatteryUnknown          Battery level undetermined.
    \value BatteryEmpty            Battery level reported as 0, system will shutdown.
    \value BatteryCritical         Battery level is critical 3% or less.
    \value BatteryVeryLow          Battery level is very low, 10% or less.
    \value BatteryLow              Battery level is low 40% or less.
    \value BatteryOk               Battery level is above 40%.
    \value BatteryFull             Battery level is 100%.
*/

/*!
    \enum QSystemBatteryInfo::ChargingState
    This enum describes the charging state:

    \value NotCharging           Battery is not charging.
    \value Charging              Battery is charging.
    \value ChargingError         Error.
*/

/*!
    \enum QSystemBatteryInfo::ChargerType
    This enum describes the type of charger used.

    \value UnknownCharger           Charger type unknown, or error.
    \value NoCharger                No charger.
    \value WallCharger              Using wall (mains) charger.
    \value USBCharger               Using USB charger when the system cannot differentiate the current.
    \value USB_500mACharger         Using USB charger at 500 mA.
    \value USB_100mACharger         Using USB charger at 100 mA.
    \value VariableCurrentCharger   Using variable current charger such as bicycle or solar.
*/

/*!
    \enum QSystemBatteryInfo::EnergyUnit
    This enum describes the energy unit used by the system.

    \value UnitUnknown            Energy unit unknown.
    \value UnitmAh                Energy described in milli Amp hours (mAh)
    \value UnitmWh                Energy described in milli watt hours (mWh)
*/

/*!
    \fn void QSystemBatteryInfo::batteryStatusChanged(QSystemBatteryInfo::BatteryStatus status)

    This signal is emitted when battery status has changed to \a status.

    \sa QSystemBatteryInfo::ChargingState
*/

/*!
    \fn void QSystemBatteryInfo::chargingStateChanged(QSystemBatteryInfo::ChargingState state)

    This signal is emitted when charging state has changed to \a state.
*/

/*!
    \fn void QSystemBatteryInfo::chargerTypeChanged(QSystemBatteryInfo::ChargerType type)

    This signal is emitted when the charger type has changed to \a type.
*/

/*!
    \fn void QSystemBatteryInfo::nominalCapacityChanged(int capacity)

    This signal is emitted when nominal (maximum) battery level has changed to \a capacity.
*/

/*!
    \fn void QSystemBatteryInfo::remainingCapacityPercentChanged(int capacity)

    This signal is emitted when battery capacity in percent has changed to \a capacity.
*/

/*!
    \fn void QSystemBatteryInfo::remainingCapacityChanged(int capacity)

    This signal is emitted when battery's remaining capacity has changed to \a capacity, which is
    reported in QSystemBatteryInfo::EnergyUnit.

    \sa QSystemBatteryInfo::energyMeasurementUnit
*/

/*!
    \fn void QSystemBatteryInfo::remainingChargingTimeChanged(int time)

    This signal is emitted when remianing charge time has changed to \a time.
*/

/*!
    \fn void QSystemBatteryInfo::currentFlowChanged(int flow)

    This signal is emitted when the current battery current has changed to \a flow.

    On some systems, listening to this signal would lead to a CPU intensive process,
    so you should disconnect from this signal when finished.
*/

/*!
    \fn void QSystemBatteryInfo::remainingCapacityBarsChanged(int bars)

    This signal is emitted when the remaining capacity in the number of bars has changed
    to \a bars.
*/

/*!
    Constructs a QSystemBatteryInfo object with the given \a parent.
*/
QSystemBatteryInfo::QSystemBatteryInfo(QObject *parent)
    : QObject(parent)
    , d(batteryInfoPrivate())
{
    qRegisterMetaType<QSystemBatteryInfo::BatteryStatus>("QSystemBatteryInfo::BatteryStatus");
    qRegisterMetaType<QSystemBatteryInfo::ChargingState>("QSystemBatteryInfo::ChargingState");
    qRegisterMetaType<QSystemBatteryInfo::ChargerType>("QSystemBatteryInfo::ChargerType");
    qRegisterMetaType<QSystemBatteryInfo::EnergyUnit>("QSystemBatteryInfo::EnergyUnit");
}

/*!
    Destroys the QSystemBatteryInfo object.
*/
QSystemBatteryInfo::~QSystemBatteryInfo()
{
}

/*!
    \property QSystemBatteryInfo::chargerType
    \brief The charger type.

    Returns the currently used charger type.
*/
QSystemBatteryInfo::ChargerType QSystemBatteryInfo::chargerType() const
{
    return batteryInfoPrivate()->chargerType();
}

/*!
    \property QSystemBatteryInfo::chargingState
    \brief The charging state.

    Returns the charging state.
*/
QSystemBatteryInfo::ChargingState QSystemBatteryInfo::chargingState() const
{
    return batteryInfoPrivate()->chargingState();
}

/*!
    \property QSystemBatteryInfo::nominalCapacity
    \brief The nominal battery capacity.

    Returns the nominal (maximum) capacity of the battery, measured in QSystemBatteryInfo::EnergyUnit.
    If no battery is found or on error, -1 is returned.
*/
int QSystemBatteryInfo::nominalCapacity() const
{
    return batteryInfoPrivate()->nominalCapacity();
}

/*!
    \property QSystemBatteryInfo::remainingCapacityPercent
    \brief The battery level in percent.

    Returns the remaining battery level of the battery in percent.
    If no battery is found or on error, -1 is returned.
*/
int QSystemBatteryInfo::remainingCapacityPercent() const
{
    return batteryInfoPrivate()->remainingCapacityPercent();
}

/*!
    \property QSystemBatteryInfo::remainingCapacity
    \brief The battery level in QSystemBatteryInfo::EnergyUnit

    Returns the remaining battery level of the battery, measured in QSystemBatteryInfo::EnergyUnit.
    If no battery is found or on error, -1 is returned.

    \sa QSystemBatteryInfo::energyMeasurementUnit()
*/
int QSystemBatteryInfo::remainingCapacity() const
{
    return batteryInfoPrivate()->remainingCapacity();
}

/*!
    \property QSystemBatteryInfo::voltage
    \brief The battery voltage.

    Returns the voltage of the battery, in millivolts (mV). If no battery is found or on error, -1
    is returned.
*/
int QSystemBatteryInfo::voltage() const
{
    return batteryInfoPrivate()->voltage();
}

/*!
    \property QSystemBatteryInfo::remainingChargingTime
    \brief The remaining time of charging

    Returns the remaining charging time in seconds. 0 is returned if the battery is full or not
    charging. If no battery is found or on error, -1 is returned.
*/
int QSystemBatteryInfo::remainingChargingTime() const
{
    return batteryInfoPrivate()->remainingChargingTime();
}

/*!
    \property QSystemBatteryInfo::currentFlow
    \brief The battery current flow.

    Returns the amount of current flowing out from the battery, in milliapmeres (mA). A positive flow
    means discharging, a negative flow means charging, and 0 is returned if on error or information is
    not available.
*/
int QSystemBatteryInfo::currentFlow() const
{
    return batteryInfoPrivate()->currentFlow();
}

/*!
    \property QSystemBatteryInfo::remainingCapacityBars
    \brief The remaining capacity in number of bars.

    Returns the remaining capacity in number of bars.
*/
int QSystemBatteryInfo::remainingCapacityBars() const
{
    return batteryInfoPrivate()->remainingCapacityBars();
}

/*!
    \property QSystemBatteryInfo::maxBars
    \brief The maximum number of bars the system uses.

    Returns the maximum number of bars the system uses. If this information is not available or on
    error, -1 is returned.
*/
int QSystemBatteryInfo::maxBars() const
{
    return batteryInfoPrivate()->maxBars();
}

/*!
    \internal
*/
void QSystemBatteryInfo::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))) {
        connect(d, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                this, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState))) {
        connect(d, SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
                this, SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType))) {
        connect(d, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
                this, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(nominalCapacityChanged(int))) {
        connect(d, SIGNAL(nominalCapacityChanged(int)),
                this, SIGNAL(nominalCapacityChanged(int)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(remainingCapacityPercentChanged(int))) {
        connect(d, SIGNAL(remainingCapacityPercentChanged(int)),
                this, SIGNAL(remainingCapacityPercentChanged(int)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(remainingCapacityChanged(int))) {
        connect(d, SIGNAL(remainingCapacityChanged(int)),
                this, SIGNAL(remainingCapacityChanged(int)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(currentFlowChanged(int))) {
        connect(d, SIGNAL(currentFlowChanged(int)),
                this, SIGNAL(currentFlowChanged(int)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(remainingCapacityBarsChanged(int))) {
        connect(d, SIGNAL(remainingCapacityBarsChanged(int)),
                this, SIGNAL(remainingCapacityBarsChanged(int)),
                Qt::UniqueConnection);
    } else if (QLatin1String(signal) == SIGNAL(remainingChargingTimeChanged(int))) {
        connect(d, SIGNAL(remainingChargingTimeChanged(int)),
                this, SIGNAL(remainingChargingTimeChanged(int)),
                Qt::UniqueConnection);
    }
}

/*!
    \internal
*/
void QSystemBatteryInfo::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))) {
        disconnect(d, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                   this, SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));
    } else if (QLatin1String(signal) == SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState))) {
        disconnect(d, SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
                   this, SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)));
    } else if (QLatin1String(signal) == SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType))) {
        disconnect(d, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
                   this, SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));
    } else if (QLatin1String(signal) == SIGNAL(nominalCapacityChanged(int))) {
        disconnect(d, SIGNAL(nominalCapacityChanged(int)),
                   this, SIGNAL(nominalCapacityChanged(int)));
    } else if (QLatin1String(signal) == SIGNAL(remainingCapacityPercentChanged(int))) {
        disconnect(d, SIGNAL(remainingCapacityPercentChanged(int)),
                   this, SIGNAL(remainingCapacityPercentChanged(int)));
    } else if (QLatin1String(signal) == SIGNAL(remainingCapacityChanged(int))) {
        disconnect(d, SIGNAL(remainingCapacityChanged(int)),
                   this, SIGNAL(remainingCapacityChanged(int)));
    } else if (QLatin1String(signal) == SIGNAL(currentFlowChanged(int))) {
        disconnect(d, SIGNAL(currentFlowChanged(int)),
                   this, SIGNAL(currentFlowChanged(int)));
    } else if (QLatin1String(signal) == SIGNAL(remainingCapacityBarsChanged(int))) {
        disconnect(d, SIGNAL(remainingCapacityBarsChanged(int)),
                   this, SIGNAL(remainingCapacityBarsChanged(int)));
    } else if (QLatin1String(signal) == SIGNAL(remainingChargingTimeChanged(int))) {
        disconnect(d, SIGNAL(remainingChargingTimeChanged(int)),
                   this, SIGNAL(remainingChargingTimeChanged(int)));
    }
}

/*!
    \property QSystemBatteryInfo::batteryStatus
    \brief The battery status.

    Returns the status of the battery.
*/
QSystemBatteryInfo::BatteryStatus QSystemBatteryInfo::batteryStatus() const
{
   return batteryInfoPrivate()->batteryStatus();
}

/*!
    \property QSystemBatteryInfo::energyMeasurementUnit
    \brief The energy unit used by the system.

    Returns the energy unit that the system uses.

    \sa QSystemBatteryInfo::EnergyUnit
*/
QSystemBatteryInfo::EnergyUnit QSystemBatteryInfo::energyMeasurementUnit() const
{
    return batteryInfoPrivate()->energyMeasurementUnit();
}

#include "moc_qsystembatteryinfo.cpp"

QTM_END_NAMESPACE
