/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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


#ifndef QDECLARATIVEBATTERYINFO_P_H
#define QDECLARATIVEBATTERYINFO_P_H

#include "qsystembatteryinfo.h"

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QDeclarativeBatteryInfo : public QSystemBatteryInfo
{
    Q_OBJECT

    Q_PROPERTY(bool monitorBatteryStatusChanges READ monitorBatteryStatusChanges WRITE startBatteryStatusChanged)
    Q_PROPERTY(bool monitorChargingStateChanges READ monitorChargingStateChanges WRITE startChargingStateChanged)
    Q_PROPERTY(bool monitorChargerTypeChanges READ monitorChargerTypeChanges WRITE startChargerTypeChanged)
    Q_PROPERTY(bool monitorNominalCapacityChanges READ monitorNominalCapacityChanges WRITE startNominalCapacityChanged)
    Q_PROPERTY(bool monitorRemainingCapacityPercentChanges READ monitorRemainingCapacityPercentChanges WRITE startRemainingCapacityPercentChanged)
    Q_PROPERTY(bool monitorRemainingCapacityChanges READ monitorRemainingCapacityChanges WRITE startRemainingCapacityChanged)
    Q_PROPERTY(bool monitorCurrentFlowChanges READ monitorCurrentFlowChanges WRITE startCurrentFlowChanged)
    Q_PROPERTY(bool monitorRemainingCapacityBarsChanges READ monitorRemainingCapacityBarsChanges WRITE startRemainingCapacityBarsChanged)
    Q_PROPERTY(bool monitorRemainingChargingTimeChanges READ monitorRemainingChargingTimeChanges WRITE startRemainingChargingTimeChanged)

public:
    explicit QDeclarativeBatteryInfo(QObject *parent = 0);


    void startBatteryStatusChanged(bool on);
    bool monitorBatteryStatusChanges();

    void startChargingStateChanged(bool on);
    bool monitorChargingStateChanges();

    void startChargerTypeChanged(bool on);
    bool monitorChargerTypeChanges();

    void startNominalCapacityChanged(bool on);
    bool monitorNominalCapacityChanges();

    void startRemainingCapacityPercentChanged(bool on);
    bool monitorRemainingCapacityPercentChanges();

    void startRemainingCapacityChanged(bool on);
    bool monitorRemainingCapacityChanges();

    void startCurrentFlowChanged(bool on);
    bool monitorCurrentFlowChanges();

    void startRemainingCapacityBarsChanged(bool on);
    bool monitorRemainingCapacityBarsChanges();

    void startRemainingChargingTimeChanged(bool on);
    bool monitorRemainingChargingTimeChanges();

protected:
    bool monitoringBatteryStatusChanges;
    bool monitoringChargingStateChanges;
    bool monitoringChargerTypeChanges;
    bool monitoringNominalCapacityChanges;
    bool monitoringRemainingCapacityPercentChanges;
    bool monitoringRemainingCapacityChanges;
    bool monitoringCurrentFlowChanges;
    bool monitoringRemainingCapacityBarsChanges;
    bool monitoringRemainingChargingTimeChanges;
};

#endif // QDECLARATIVEBATTERYINFO_P_H
