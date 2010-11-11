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
     \value ChargingFailed        Error.
  */
/*!
    \enum QSystemBatteryInfo::ChargerType
    This enum describes the type of charger used.

    \value Unknown                Charger type unknown, or error.
    \value None                   No charger.
    \value Wall                   Using wall (mains) charger.
    \value USB_500mA              Using USB charger at 500 mA.
    \value USB_100mA              Using USB charger at 100 mA.
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
  \fn void QSystemBatteryInfo::remainingCapacitymAhChanged(int level)

  This signal is emitted when battery capacity in mAh (milliampere-hour) has changed.
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

  This signal is emitted when battery level has changed.
  \a level is the new level.
 */
/*!
  \fn void QSystemBatteryInfo::cumulativeCurrentFlowChanged(int level)

  This signal is emitted when battery level has changed.
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
    QObject(parent)
{
    qRegisterMetaType<QSystemBatteryInfo::BatteryStatus>("QSystemBatteryInfo::BatteryStatus");
    qRegisterMetaType<QSystemBatteryInfo::ChargingState>("QSystemBatteryInfo::ChargingState");
    qRegisterMetaType<QSystemBatteryInfo::ChargerType>("QSystemBatteryInfo::ChargerType");

#if defined(Q_OS_LINUX) && !defined(QT_SIMULATOR)
    d = new QSystemBatteryInfoPrivate(static_cast<QSystemBatteryInfoLinuxCommonPrivate*>(parent));
#else
    d = new QSystemBatteryInfoPrivate(parent);
#endif
}

/*!
  Destroys the QSystemBatteryInfo object.
 */
QSystemBatteryInfo::~QSystemBatteryInfo()
{
    delete d;
}

/*!
    Returns the currently used charger type.
  */
QSystemBatteryInfo::ChargerType QSystemBatteryInfo::chargerType() const
{
    return d->chargerType();
}

/*!
    Returns the charging state.
  */
QSystemBatteryInfo::ChargingState QSystemBatteryInfo::chargingState() const
{
    return d->chargingState();
}

/*!
  \property QSystemBatteryInfo::nominalCapacity
  \brief The nominal battery capacity.

    Returns the nominal (maximum) capacity of the battery, in milliampere-hours (mAh).
*/
int QSystemBatteryInfo::nominalCapacity() const
{
    return d->nominalCapacity();
}

/*!
  \property QSystemBatteryInfo::remainingCapacityPercent
  \brief The battery level in percent.

    Returns the remaining battery level of the battery in percent.
  */
int QSystemBatteryInfo::remainingCapacityPercent() const
{
    return d->remainingCapacityPercent();
}

/*!
  \property QSystemBatteryInfo::remainingCapacitymAh
  \brief The battery level in milliampere-hour (mAh).

    Returns the remaining battery level of the battery in milliampere-hours (mAh).
  */
int QSystemBatteryInfo::remainingCapacitymAh() const
{
    return d->remainingCapacitymAh();
}

/*!
  \property QSystemBatteryInfo::voltage
  \brief The battery voltage.

    Returns the voltage of the battery, in millivolts (mV).
  */
int QSystemBatteryInfo::voltage() const
{
    return d->voltage();
}

/*!
  \property QSystemBatteryInfo::remainingChargingTime
  \brief The remaining time of charging

    Returns the remaining time of charging in seconds if charging,
    or -1 if not charging.
*/
int QSystemBatteryInfo::remainingChargingTime() const
{
    return d->remainingChargingTime();
}

/*!
  \property QSystemBatteryInfo::currentFlow
  \brief The battery current flow.

    Returns the amount of current flowing out from the battery (a short term averge), milliapmeres (mA).
    Positive current means discharging and negative current means charging.
  */
int QSystemBatteryInfo::currentFlow() const
{
    return d->currentFlow();
}

/*!
  \property QSystemBatteryInfo::cumulativeCurrentFlow
  \brief The cumulative current flow.

    Returns the cumulative amount of current flowing out from the battery (the coulomb counter),
    in milliampere-seconds (mAs).


    The reference point of the cumulative battery current is undefined and only differences
    of the returned values are meaningful.

    This function needs to be called twice and the time between calls needs to be known to be able
    to measure the delta of the inflow/outflow. If the delta is positive, it means that battery
    has discharged that much. If it is negative, it has charged by that much.
*/
int QSystemBatteryInfo::cumulativeCurrentFlow() const
{
    return d->cumulativeCurrentFlow();
}

/*!
  \property QSystemBatteryInfo::remainingCapacityBars
  \brief The remaining capacity in number of bars.

   Returns the remaining capacity in number of bars.
   The bar count will not necessarily always reflect one to one on the remaining capacity percentage.
   */
int QSystemBatteryInfo::remainingCapacityBars() const
{
    return d->remainingCapacityBars();
}

/*!
  \property QSystemBatteryInfo::maxBars
  \brief The maximum number of bars the system uses.

   Returns the Maximum number of bars the system uses.
  */
int QSystemBatteryInfo::maxBars() const
{
    return d->maxBars();
}

/*!
  \internal
  This function is called when the client connects to signals.

  \sa connectNotify()
*/

void QSystemBatteryInfo::connectNotify(const char *signal)
{
}

/*!
 \internal
 This function is called when the client disconnects to signals.

 \sa connectNotify()
*/

void QSystemBatteryInfo::disconnectNotify(const char *signal)
{

}

QSystemBatteryInfo::BatteryStatus QSystemBatteryInfo::batteryStatus() const
{
   return d->batteryStatus();
}

#include "moc_qsystembatteryinfo.cpp"

QTM_END_NAMESPACE
