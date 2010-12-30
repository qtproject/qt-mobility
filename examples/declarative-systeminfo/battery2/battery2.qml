/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import QtMobility.systeminfo 1.1

Rectangle {
    id: screen
    width: 360
    height: 360


    BatteryInfo {
        id: batinfo;

        onChargerTypeChanged:  {
            if(batinfo.chargerType == -1) {
                chargertype.text = "Unknown Charger"
            }
            if(batinfo.chargerType == 0) {
                chargertype.text = "No Charger"
            }
            if(batinfo.chargerType == 1) {
                chargertype.text = "Wall Charger"
            }
            if(batinfo.chargerType == 2) {
                chargertype.text = "USB Charger"
            }
            if(batinfo.chargerType == 3) {
                chargertype.text = "USB 500 mA Charger"
            }
            if(batinfo.chargerType == 4) {
                chargertype.text = "USB 100 mA Charger"
            }
            if(batinfo.chargerType == 5) {
                chargertype.text = "Variable Current Charger"
            }
        }

        onChargingStateChanged: {
            if(batinfo.chargingState == -1) {
                chargeState.text = "Charging State: Not Charging / Error"
            }
            if(batinfo.chargingState == 0) {
                chargeState.text = "Charging State: Not Charging"
            }
            if(batinfo.chargingState == 1) {
                chargeState.text = "Charging State: Charging"
            }
        }
        onBatteryStatusChanged: {
            if(batinfo.batteryStatus == -1) {
                batStat.text = "Battery Unknown"
            }
            if(batinfo.batteryStatus == 0) {
                batStat.text = "Battery Empty"
            }
            if(batinfo.batteryStatus == 1) {
                batStat.text = "Battery Critical"
            }
            if(batinfo.batteryStatus == 2) {
                batStat.text = "Battery Very Low"
            }
            if(batinfo.batteryStatus == 3) {
                batStat.text = "Battery Low"
            }
            if(batinfo.batteryStatus == 4) {
                batStat.text = "Battery Ok"
            }
            if(batinfo.batteryStatus == 5) {
                batStat.text = "Battery Full"
            }
        }
        onVoltageChanged: { voltagetext.text = "Voltage: "+ voltage}
        onRemainingCapacityPercentChanged: { leveltext.text = "Level: "+ remainingCapacityPercent }
        onRemainingCapacityChanged: { remCap.text = "Remaining Capacity: "+ batinfo.remainingCapacity; }

        onRemainingChargingTimeChanged: { chargeTime.text = "Charge time: "+ batinfo.remainingChargingTime; }
        onCurrentFlowChanged: { curFLow.text = "Current Energy: "+ batinfo.currentFlow; }
    property alias batState : batinfo.chargingState
    }

    Text {
        id: leveltext
        anchors.centerIn: parent
        text: "Level "+batinfo.remainingCapacityPercent
    }
    Text {
        id: voltagetext
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: leveltext.bottom}
        text: "Voltage: "+ batinfo.voltage
    }
    Text {
        id: nomCap
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: voltagetext.bottom}
        text: "Nominal Capacity: "+ batinfo.nominalCapacity
    }
    Text {
        id: remCap
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: nomCap.bottom}
        text: "Remaining Capacity: "+ batinfo.remainingCapacity
    }
    Text {
        id: chargeTime
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: remCap.bottom}
        text: "Charge time: "+ batinfo.remainingChargingTime;

    }
    Text {
        id: curFLow
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: chargeTime.bottom}
        text: "Current Energy: "+ batinfo.currentFlow
    }
    Text {
        id: batStat
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: curFLow.bottom}
        text: {
            if(batinfo.batteryStatus == -1) {
                batStat.text = "Battery Unknown"
            }
            if(batinfo.batteryStatus == 0) {
                batStat.text = "Battery Empty"
            }
            if(batinfo.batteryStatus == 1) {
                batStat.text = "Battery Critical"
            }
            if(batinfo.batteryStatus == 2) {
                batStat.text = "Battery Very Low"
            }
            if(batinfo.batteryStatus == 3) {
                batStat.text = "Battery Low"
            }
            if(batinfo.batteryStatus == 4) {
                batStat.text = "Battery Ok"
            }
            if(batinfo.batteryStatus == 5) {
                batStat.text = "Battery Full"
            }
        }
    }

    Text {
        id: energyUnit
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: batStat.bottom}
        text: {
            if(batinfo.energyMeasurementUnit == -1) {
                energyUnit.text = "Unknown energy unit"
            }
            if(batinfo.energyMeasurementUnit == 0) {
                energyUnit.text = "mAh"
            }
            if(batinfo.energyMeasurementUnit == 1) {
                energyUnit.text = "mWh"
            }
        }
    }

    Text {
        id: chargertype
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: energyUnit.bottom}
        text: {
            if(batinfo.chargerType == -1) {
                chargertype.text = "Unknown Charger"
            }
            if(batinfo.chargerType == 0) {
                chargertype.text = "No Charger"
            }
            if(batinfo.chargerType == 1) {
                chargertype.text = "Wall Charger"
            }
            if(batinfo.chargerType == 2) {
                chargertype.text = "USB Charger"
            }
            if(batinfo.chargerType == 3) {
                chargertype.text = "USB 500 mA Charger"
            }
            if(batinfo.chargerType == 4) {
                chargertype.text = "USB 100 mA Charger"
            }
            if(batinfo.chargerType == 5) {
                chargertype.text = "Variable Current Charger"
            }
        }
    }
    Text {
        id: chargeState
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: chargertype.bottom}
        text: {
            if(batinfo.chargingState == -1) {
                chargeState.text = "Charging Unknown"
            }
            if(batinfo.chargingState == 0) {
                chargeState.text = "Discharging"
            }
            if(batinfo.chargingState == 1) {
                chargeState.text = "Charging"
            }
        }
    }

    Component.onCompleted: {
        batinfo.startRemainingCapacityPercentChanged();
        batinfo.startChargingStateChanged();
        batinfo.startRemainingChargingTimeChanged;
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
