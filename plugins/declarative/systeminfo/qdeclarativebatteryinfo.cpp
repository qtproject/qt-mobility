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

    This element is part of the \bold{QtMobility.systeminfo 1.1} module.
    It is a convience class to make QML usage easier.

    Note: To use notification signals, you need to use the start* slots.


    \qml
        Component.onCompleted: {
            batteryinfo.startRemainingCapacityPercentChanged();
            batteryinfo.startBatteryStatusChanged();
        }
    \endqml

\sa QSystemBatteryInfo
*/

QDeclarativeBatteryInfo::QDeclarativeBatteryInfo(QObject *parent) :
    QSystemBatteryInfo(parent)
{
   startConnections();
}

void QDeclarativeBatteryInfo::startConnections()
{
    startBatteryStatusChanged();

    startChargingStateChanged();
    startChargerTypeChanged();

    startNominalCapacityChanged();
    startRemainingCapacityPercentChanged();
    startRemainingCapacityChanged();

    startCurrentFlowChanged();
    startRemainingCapacityBarsChanged();
    startRemainingChargingTimeChanged();
    startVoltageChanged();
}

void QDeclarativeBatteryInfo::startBatteryStatusChanged()
{
    connect(batteryInfo(),SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),
            this,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startChargingStateChanged()
{
    connect(batteryInfo(),SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),
            this,SIGNAL(chargingStateChanged(QSystemBatteryInfo::ChargingState)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startChargerTypeChanged()
{
    connect(batteryInfo(),SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),
            this,SIGNAL(chargerTypeChanged(QSystemBatteryInfo::ChargerType)),Qt::UniqueConnection);
}


void QDeclarativeBatteryInfo::startNominalCapacityChanged()
{
    connect(batteryInfo(),SIGNAL(nominalCapacityChanged(int)),
            this,SIGNAL(nominalCapacityChanged(int)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startRemainingCapacityPercentChanged()
{
    connect(batteryInfo(),SIGNAL(remainingCapacityPercentChanged(int)),
            this,SIGNAL(remainingCapacityPercentChanged(int)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startRemainingCapacityChanged()
{
    connect(batteryInfo(),SIGNAL(remainingCapacityChanged(int)),
            this,SIGNAL(remainingCapacityChanged(int)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startCurrentFlowChanged()
{
    connect(batteryInfo(),SIGNAL(currentFlowChanged(int)),
            this,SIGNAL(currentFlowChanged(int)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startRemainingCapacityBarsChanged()
{
    connect(batteryInfo(),SIGNAL(remainingCapacityBarsChanged(int)),
            this,SIGNAL(remainingCapacityBarsChanged(int)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startRemainingChargingTimeChanged()
{
    connect(batteryInfo(),SIGNAL(remainingChargingTimeChanged(int)),
            this,SIGNAL(remainingChargingTimeChanged(int)),Qt::UniqueConnection);
}

void QDeclarativeBatteryInfo::startVoltageChanged()
{
    connect(batteryInfo(),SIGNAL(voltageChanged(int)),
            this,SIGNAL(voltageChanged(int)),Qt::UniqueConnection);
}

