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


#include "qdeclarativebatteryinfo_p.h"
#include "qsystembatteryinfo.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemBatteryInfo, batteryInfo)

/*!
    \qmlclass BatteryInfo QDeclarativeBatteryInfo
    \brief The BatteryInfo element allows you to receive battery change notifications from the device.

    \inherits QSystemBatteryInfo

    \ingroup qml-systeminfo

    This element is part of the \bold{QtMobility.systeminfo 1.2} module.
    It is a convience class to make QML usage easier.

    Note: To use notification signals, you need to set the monitor* properties to true.


    \qml
        monitorChargerTypeChanges: true
        monitorChargingStateChanges: true
    \endqml

\sa QSystemBatteryInfo
*/
QDeclarativeBatteryInfo::QDeclarativeBatteryInfo(QObject *parent) :
    QSystemBatteryInfo(parent)
{
}


/*!
   This function starts the startBatteryStatusChanged notification.

*/
void QDeclarativeBatteryInfo::startBatteryStatusChanged(bool on)
{
    monitoringBatteryStatusChanges = on;
    if(on) {
        connect(batteryInfo(),SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
                this,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)));
    }
}

/*!

  \qmlproperty bool BatteryInfo::monitorBatteryStatusChanges()

  Start the connection for the batteryStatusChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorBatteryStatusChanges()
{
    return monitoringBatteryStatusChanges;
}

/*!
   This function starts the startChargingStateChanged notification.

*/
void QDeclarativeBatteryInfo::startChargingStateChanged(bool on)
{
    monitoringChargingStateChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
            this,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
                this,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorChargingStateChanges
    Start the connection for the chargingStateChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorChargingStateChanges()
{
    return monitoringChargingStateChanges;
}

/*!
   This function starts the startChargerTypeChanged notification.

*/
void QDeclarativeBatteryInfo::startChargerTypeChanged(bool on)
{
    monitoringChargerTypeChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
            this,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
                this,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorChargerTypeChanges
    Start the connection for the chargerTypeChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorChargerTypeChanges()
{
    return monitoringChargerTypeChanges;
}

/*!
   This function starts the startNominalCapacityChanged notification.

*/
void QDeclarativeBatteryInfo::startNominalCapacityChanged(bool on)
{
    monitoringNominalCapacityChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(nominalCapacityChanged(int)),
            this,SIGNAL(nominalCapacityChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(nominalCapacityChanged(int)),
                this,SIGNAL(nominalCapacityChanged(int)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorNominalCapacityChanges
    Start the connection for the nominalCapacityChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorNominalCapacityChanges()
{
    return monitoringNominalCapacityChanges;
}

/*!
   This function starts the startRemainingCapacityPercentChanged notification.

*/
void QDeclarativeBatteryInfo::startRemainingCapacityPercentChanged(bool on)
{
    monitoringRemainingCapacityPercentChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(remainingCapacityPercentChanged(int)),
            this,SIGNAL(remainingCapacityPercentChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(remainingCapacityPercentChanged(int)),
                this,SIGNAL(remainingCapacityPercentChanged(int)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorRemainingCapacityPercentChanges
    Start the connection for the remainingCapacityPercentChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorRemainingCapacityPercentChanges()
{
    return monitoringRemainingCapacityPercentChanges;
}

/*!
   This function starts the startRemainingCapacityChanged notification.

*/
void QDeclarativeBatteryInfo::startRemainingCapacityChanged(bool on)
{
    monitoringRemainingCapacityChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(remainingCapacityChanged(int)),
            this,SIGNAL(remainingCapacityChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(remainingCapacityChanged(int)),
                this,SIGNAL(remainingCapacityChanged(int)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorRemainingCapacityChanges
    Start the connection for the remainingCapacityChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorRemainingCapacityChanges()
{
    return monitoringRemainingCapacityChanges;
}

/*!
   This function starts the startCurrentFlowChanged notification.

*/
void QDeclarativeBatteryInfo::startCurrentFlowChanged(bool on)
{
    monitoringCurrentFlowChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(currentFlowChanged(int)),
            this,SIGNAL(currentFlowChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(currentFlowChanged(int)),
                this,SIGNAL(currentFlowChanged(int)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorCurrentFlowChanges
    Start the connection for the currentFlowChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorCurrentFlowChanges()
{
    return monitoringCurrentFlowChanges;
}

/*!
   This function starts the startRemainingCapacityBarsChanged notification.

*/
void QDeclarativeBatteryInfo::startRemainingCapacityBarsChanged(bool on)
{
    monitoringRemainingCapacityBarsChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(remainingCapacityBarsChanged(int)),
            this,SIGNAL(remainingCapacityBarsChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(remainingCapacityBarsChanged(int)),
                this,SIGNAL(remainingCapacityBarsChanged(int)));
    }
}

/*!
    \qmlproperty bool BatteryInfo::monitorRemainingCapacityBarsChanges
    Start the connection for the remainingCapacityBarsChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorRemainingCapacityBarsChanges()
{
    return monitoringRemainingCapacityBarsChanges;
}

/*!
   This function starts the startRemainingChargingTimeChanged notification.

*/
void QDeclarativeBatteryInfo::startRemainingChargingTimeChanged(bool on)
{
    monitoringRemainingChargingTimeChanges = on;
    if(on) {
    connect(batteryInfo(),SIGNAL(remainingChargingTimeChanged(int)),
            this,SIGNAL(remainingChargingTimeChanged(int)),Qt::UniqueConnection);
    } else {
        disconnect(batteryInfo(),SIGNAL(remainingChargingTimeChanged(int)),
                this,SIGNAL(remainingChargingTimeChanged(int)));
    }
}
/*!
    \qmlproperty bool BatteryInfo::monitorRemainingChargingTimeChanges
    Start the connection for the remainingChargingTimeChanged signal.
  */
bool QDeclarativeBatteryInfo::monitorRemainingChargingTimeChanges()
{
    return monitoringRemainingChargingTimeChanges;
}
/*!
    \qmlsignal BatteryInfo::batteryStatusChanged(BatteryInfo::BatteryStatus)

  This signal is emitted when battery status has changed.

  \sa QSystemBatteryInfo::ChargingState
*/
/*!
    \qmlsignal BatteryInfo::chargingStateChanged(BatteryInfo::ChargingState)
   This signal is emitted when charging state has changed.

    \sa QSystemBatteryInfo::ChargingState
*/
/*!
    \qmlsignal BatteryInfo::chargerTypeChanged(BatteryInfo::ChargerType)
This signal is emitted when the charger type has changed, such as when a phone gets plugged in to the wall, or usb.

 \sa QSystemBatteryInfo::ChargerType
*/
/*!
    \qmlsignal BatteryInfo::nominalCapacityChanged(int)
    This signal is emitted when nominal (maximum) battery level has changed.
 \sa QSystemBatteryInfo::nominalCapacity
*/
/*!
    \qmlsignal BatteryInfo::remainingCapacityPercentChanged(int)
  This signal is emitted when battery capacity in percent has changed.
 \sa QSystemBatteryInfo::remainingCapacityPercent
*/
/*!
    \qmlsignal BatteryInfo::remainingCapacityChanged
  This signal is emitted when battery capacity has changed, reported in QSystemBatteryInfo::EnergyUnit.

 \sa QSystemBatteryInfo::remainingCapacity
*/

/*!
    \qmlsignal BatteryInfo::currentFlowChanged(int)

  This signal is emitted when the short term averge battery current has changed,
  or on some systems at regular intervals.

  On some systems where this can lead to a CPU intensive process, you should disconnect
  from this signal when you are finished.

 \sa QSystemBatteryInfo::currentFlow
*/
/*!
    \qmlsignal BatteryInfo::remainingCapacityBarsChanged(int)
  This signal is emitted when battery level has changed.

 \sa QSystemBatteryInfo::remainingCapacityBars
*/
/*!
    \qmlsignal BatteryInfo::remainingChargingTimeChanged(int)
This signal is emitted when remianing charge time has changed.

 \sa QSystemBatteryInfo::remainingChargingTime
*/


/*!
  \qmlproperty ChargerType BatteryInfo::chargerType

    Returns the currently used charger type.
  */

/*!
  \qmlproperty ChargingState BatteryInfo::chargingState
    Returns the charging state.
  */
/*!
  \qmlproperty int BatteryInfo::nominalCapacity

    Returns the nominal (maximum) capacity of the battery, in milliampere-hours (mAh).
    If no battery is found, -1.
*/
/*!
  \qmlproperty int BatteryInfo::remainingCapacityPercent

    Returns the remaining battery level of the battery in percent.
    If no battery is found, -1.
  */
/*!
  \qmlproperty int BatteryInfo::remainingCapacity
    Returns the remaining battery level of the battery in BatteryInfo::EnergyUnit.

    \sa QSystemBatteryInfo::EnergyUnit
  */
/*!
  \qmlproperty int BatteryInfo::voltage

    Returns the voltage of the battery, in millivolts (mV).
    If no battery is found, -1.
  */

/*!
  \qmlproperty int BatteryInfo::remainingChargingTime

    Returns the remaining time of charging in seconds if charging,
    0 if battery is full and not charging, or -1 no battery found.
*/
/*!
  \qmlproperty int BatteryInfo::currentFlow

    Returns the amount of current flowing out from the battery (a short term averge), milliapmeres (mA).
    Positive current means discharging and negative current means charging.

  */
/*!
  \qmlproperty int BatteryInfo::remainingCapacityBars

   Returns the remaining capacity in number of bars.
   The bar count will not necessarily always reflect one to one on the remaining capacity percentage.
   */

/*!
  \qmlproperty int BatteryInfo::maxBars

   Returns the Maximum number of bars the system uses. In the case that the system has no
   default number of battery bars, 0 is returned.
  */
/*!
    \qmlproperty BatteryStatus BatteryInfo::batteryStatus

    Returns the battery charge status.
  */
/*!
    \qmlproperty EnergyUnit BatteryInfo::energyMeasurementUnit

  Returns the QSystemBatteryInfo::EnergyUnit that the system uses.
  */



