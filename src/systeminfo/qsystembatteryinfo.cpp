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


#include "qsystembatteryinfo.h"
#include "qsysteminfocommon_p.h"
#include <QMetaType>

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemBatteryInfoPrivate, batteryInfoPrivate)

#ifdef QT_SIMULATOR
QSystemBatteryInfoPrivate *getSystemBatteryInfoPrivate() { return d; }
#endif

/*!
       \class QSystemBatteryInfo
       \ingroup systeminfo
       \inmodule QtSystemInfo
       \brief The QSystemBatteryInfo class provides access to battery and power information from the system.
*/
/*!
    \enum QSystemBatteryInfo::BatteryStatus
    This enum describes the status of the main battery.

    \value BatteryStatus           Battery level undetermined.
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

      \value UnitmAh                Energy described in milli Amp hours (mAh)
      \value UnitmWh                Energy described in milli watt hours (mWh)
*/
/*!

  \fn void QSystemBatteryInfo::batteryLevelChanged(int level)

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */
/*!
  \fn void QSystemBatteryInfo::chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState)

  This signal is emitted when charging state has changed.
  \a chargingState is the new chargingState.
 */
/*!
\fn void QSystemBatteryInfo::chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType)

This signal is emitted when the charger type has changed, such as when a phone gets plugged in to the wall, or usb.
\a chargerType is the new charger type.
*/
/*!
  \fn void QSystemBatteryInfo::nominalCapacityChanged(int level)

  This signal is emitted when nominal (maximum) battery level has changed.
  \a level is the new level.
 */
/*!
  \fn void QSystemBatteryInfo::remainingCapacityPercentChanged(int level)

  This signal is emitted when battery capacity in percent has changed.
  \a level is the new level.
 */
/*!
  \fn void QSystemBatteryInfo::remainingCapacityChanged(int level)

  This signal is emitted when battery capacity has changed, reported in QSystemBatteryInfo::EnergyUnit.
  \a level is the new level.
 */

/*!
  \fn void QSystemBatteryInfo::voltageChanged(int level)

  This signal is emitted when battery voltage level has changed, in millivolts (mV).
  \a level is the new level.
 */
/*!
  \fn void QSystemBatteryInfo::remainingChargingTimeChanged(int level)

  This signal is emitted when remianing charge time has changed.
  \a level is the new level.
 */
/*!
  \fn void QSystemBatteryInfo::currentFlowChanged(int level)

  This signal is emitted when the short term averge battery current has changed.
  Sent at desired interval when battery current measurement is enabled .
\sa QSystemBatteryInfo::startCurrentMeasurement

  \a level is the new level.
 */

/*!
  \fn void QSystemBatteryInfo::remainingCapacityBarsChanged(int level)

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */

/*!
   \fn QSystemBatteryInfo::QSystemBatteryInfo(QObject *parent)
   Constructs a QSystemBatteryInfo object with the given \a parent.
  */
QSystemBatteryInfo::QSystemBatteryInfo(QObject *parent) :
    QObject(parent),d(batteryInfoPrivate())
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
    Returns the currently used charger type.
  */
QSystemBatteryInfo::ChargerType QSystemBatteryInfo::chargerType() const
{
    return batteryInfoPrivate()->chargerType();
}

/*!
    Returns the charging state.
  */
QSystemBatteryInfo::ChargingState QSystemBatteryInfo::chargingState() const
{
    return batteryInfoPrivate()->chargingState();
}

/*!
  \property QSystemBatteryInfo::nominalCapacity
  \brief The nominal battery capacity.

    Returns the nominal (maximum) capacity of the battery, in milliampere-hours (mAh).
*/
int QSystemBatteryInfo::nominalCapacity() const
{
    return batteryInfoPrivate()->nominalCapacity();
}

/*!
  \property QSystemBatteryInfo::remainingCapacityPercent
  \brief The battery level in percent.

    Returns the remaining battery level of the battery in percent.
  */
int QSystemBatteryInfo::remainingCapacityPercent() const
{
    return batteryInfoPrivate()->remainingCapacityPercent();
}

/*!
  \property QSystemBatteryInfo::remainingCapacity
  \brief The battery level in QSystemBatteryInfo::EnergyUnit

    Returns the remaining battery level of the battery in QSystemBatteryInfo::EnergyUnit.

    \sa QSystemBatteryInfo::energyMeasurementUnit()
  */
int QSystemBatteryInfo::remainingCapacity() const
{
    return batteryInfoPrivate()->remainingCapacity();
}

/*!
  \property QSystemBatteryInfo::voltage
  \brief The battery voltage.

    Returns the voltage of the battery, in millivolts (mV).
  */
int QSystemBatteryInfo::voltage() const
{
    return batteryInfoPrivate()->voltage();
}

/*!
  \property QSystemBatteryInfo::remainingChargingTime
  \brief The remaining time of charging

    Returns the remaining time of charging in seconds if charging,
    or -1 if not charging.
*/
int QSystemBatteryInfo::remainingChargingTime() const
{
    return batteryInfoPrivate()->remainingChargingTime();
}

/*!
  \property QSystemBatteryInfo::currentFlow
  \brief The battery current flow.

    Returns the amount of current flowing out from the battery (a short term averge), milliapmeres (mA).
    Positive current means discharging and negative current means charging.
  */
int QSystemBatteryInfo::currentFlow() const
{
    return batteryInfoPrivate()->currentFlow();
}


/*!
  \property QSystemBatteryInfo::remainingCapacityBars
  \brief The remaining capacity in number of bars.

   Returns the remaining capacity in number of bars.
   The bar count will not necessarily always reflect one to one on the remaining capacity percentage.
   */
int QSystemBatteryInfo::remainingCapacityBars() const
{
    return batteryInfoPrivate()->remainingCapacityBars();
}

/*!
  \property QSystemBatteryInfo::maxBars
  \brief The maximum number of bars the system uses.

   Returns the Maximum number of bars the system uses.
  */
int QSystemBatteryInfo::maxBars() const
{
    return batteryInfoPrivate()->maxBars();
}

/*!
  \internal
  This function is called when the client connects to signals.

  \sa connectNotify()
*/

void QSystemBatteryInfo::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryLevelChanged(int))))) {
        connect(d,SIGNAL(batteryLevelChanged(int)),
                this,SIGNAL(batteryLevelChanged(int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))))) {
        connect(d,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargingStateChanged(QSystemBatteryInfo::ChargingState))))) {
        connect(d,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
                this,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargerTypeChanged(QSystemBatteryInfo::ChargerType))))) {
        connect(d,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
                this,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            nominalCapacityChanged(int))))) {
        connect(d,SIGNAL(nominalCapacityChanged(int)),
                this,SIGNAL(nominalCapacityChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityPercentChanged(int))))) {
        connect(d,SIGNAL(remainingCapacityPercentChanged(int)),
                this,SIGNAL(remainingCapacityPercentChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityChanged(int))))) {
        connect(d,SIGNAL(remainingCapacityChanged(int)),
                this,SIGNAL(remainingCapacityChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentFlowChanged(int))))) {
        connect(d,SIGNAL(currentFlowChanged(int)),
                this,SIGNAL(currentFlowChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityBarsChanged(int))))) {
        connect(d,SIGNAL(remainingCapacityBarsChanged(int)),
                this,SIGNAL(remainingCapacityBarsChanged(int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingChargingTimeChanged(int))))) {
        connect(d,SIGNAL(remainingChargingTimeChanged(int)),
                this,SIGNAL(remainingChargingTimeChanged(int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            voltageChanged(int))))) {
        connect(d,SIGNAL(voltageChanged(int)),
                this,SIGNAL(voltageChanged(int)));
    }
}

/*!
 \internal
 This function is called when the client disconnects to signals.

 \sa connectNotify()
*/

void QSystemBatteryInfo::disconnectNotify(const char *signal)
{

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))))) {
        disconnect(d,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargingStateChanged(QSystemBatteryInfo::ChargingState))))) {
        disconnect(d,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
                this,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargerTypeChanged(QSystemBatteryInfo::ChargerType))))) {
        disconnect(d,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
                this,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            nominalCapacityChanged(int))))) {
        disconnect(d,SIGNAL(nominalCapacityChanged(int)),
                this,SIGNAL(nominalCapacityChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityPercentChanged(int))))) {
        disconnect(d,SIGNAL(remainingCapacityPercentChanged(int)),
                this,SIGNAL(remainingCapacityPercentChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityChanged(int))))) {
        disconnect(d,SIGNAL(remainingCapacityChanged(int)),
                this,SIGNAL(remainingCapacityChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentFlowChanged(int))))) {
        disconnect(d,SIGNAL(currentFlowChanged(int)),
                this,SIGNAL(currentFlowChanged(int)));
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityBarsChanged(int))))) {
        disconnect(d,SIGNAL(remainingCapacityBarsChanged(int)),
                this,SIGNAL(remainingCapacityBarsChanged(int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingChargingTimeChanged(int))))) {
        disconnect(d,SIGNAL(remainingChargingTimeChanged(int)),
                this,SIGNAL(remainingChargingTimeChanged(int)));
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            voltageChanged(int))))) {
        disconnect(d,SIGNAL(voltageChanged(int)),
                this,SIGNAL(voltageChanged(int)));
    }
}

/*!
    \property QSystemBatteryInfo::batteryStatus
    \brief The battery status.

    Returns the battery charge status.
  */
QSystemBatteryInfo::BatteryStatus QSystemBatteryInfo::batteryStatus() const
{
   return batteryInfoPrivate()->batteryStatus();
}

/*!
  Starts battery current measurement with given \a rate.
  Return value is actual rate used by the system.
  */
int QSystemBatteryInfo::startCurrentMeasurement(int rate)
{
    return batteryInfoPrivate()->startCurrentMeasurement(rate);

}

/*!
  Returns the QSystemBatteryInfo::EnergyUnit that the system uses.
  */

QSystemBatteryInfo::EnergyUnit QSystemBatteryInfo::energyMeasurementUnit() const
{
    return batteryInfoPrivate()->energyMeasurementUnit();

}

#include "moc_qsystembatteryinfo.cpp"

QTM_END_NAMESPACE
