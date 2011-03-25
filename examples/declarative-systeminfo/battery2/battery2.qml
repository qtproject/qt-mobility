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
import Qt.labs.particles 1.0

Rectangle {
    id: screen
    width: 360
    height: 640
    property alias batlevel: batinfo.battlevel;
    property int speed: level2Speed(batlevel);
    property bool hasBattery: (batinfo.batteryStatus != -1)

    BatteryInfo {
        id: batinfo;

        property int battlevel: remainingCapacityPercent;
        property string oldstate;

        monitorChargerTypeChanges: true
        monitorChargingStateChanges: true
        monitorBatteryStatusChanges: true
        monitorRemainingCapacityPercentChanges: true
        monitorRemainingCapacityChanges: true
        monitorRemainingChargingTimeChanges: true
        monitorCurrentFlowChanges: true

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
            getPowerState();
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
                img.width = 20; img.height = 20;
            }
            if(batinfo.batteryStatus == 4) {
                batStat.text = "Battery Ok"
            }
            if(batinfo.batteryStatus == 5) {
                batStat.text = "Battery Full"
            }
        }
        onRemainingCapacityPercentChanged: doBatteryLevelChange(level)
        onRemainingChargingTimeChanged: { chargeTime.text = "Time to full: "+ minutesToFull() +" minutes"; }

        property alias batState : batinfo.chargingState

        Component.onCompleted: getPowerState();
    }

    function minutesToFull() {
        if (batinfo.remainingChargingTime > 0) {
          return (batinfo.remainingChargingTime/60.00)
        }
        return 0;
    }

    function level2Speed(level) {
        if(level > 90) {
            return 1000;
        } else if(level > 70) {
            return 1500;
        } else if(level > 60) {
            return 2000;
        } else if(level > 50) {
            return 2500;
        } else if(level > 40) {
            return 3000;
        } else if(level > 10) {
            return 3500;
        } else if(level < 11) {
            return 4000;
        }
    }

    function doBatteryLevelChange(level) {
        leveltext.text = "Level: "+ level +"%"
        floorParticles.burst(level);
        batlevel = level;
        batinfo.oldstate = img.state;
        img.state = "levelchange"
        img.state = batinfo.oldstate;
        getPowerState();
    }

    function getPowerState() {
        if (batinfo.chargingState == 0) {
            chargeState.text = "Charging State: Not Charging"
            if (batinfo.chargerType == 0) {
                img.state = "Battery"
                batinfo.oldstate = img.state;
            } else {
                img.state = "WallPower"
                batinfo.oldstate = img.state;
            }
        }
        if(batinfo.chargingState == 1) {
            chargeState.text = "Charging State: Charging"
            img.state = "Charging"
            batinfo.oldstate = img.state;
        }
    }

    Text {
        id: leveltext
        anchors.centerIn: parent
        text: "Level: "+batinfo.remainingCapacityPercent +"%"
    }
    Text {
        id: voltagetext
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: leveltext.bottom}
        text: "Voltage: "+ batinfo.voltage +" mV"
    }
    Text {
        id: nomCap
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: voltagetext.bottom}
        text: "Nominal Capacity: "+ batinfo.nominalCapacity +" "+getEnergyUnit()
    }
    Text {
        id: remCap
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: nomCap.bottom}
        text: "Remaining Capacity: "+ batinfo.remainingCapacity +" "+getEnergyUnit()
    }
    Text {
        id: chargeTime
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: remCap.bottom}
        text: "Time to full: "+ minutesToFull() +" minutes";

    }
    Text {
        id: curFLow
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: chargeTime.bottom}
        text: "Current Energy: "+ batinfo.currentFlow +" mA"
    }

    function getEnergyUnit() {
        if (batinfo.energyMeasurementUnit == -1) {
            return "Unknown energy unit"
        }
        if (batinfo.energyMeasurementUnit == 0) {
            return "mAh"
        }
        if (batinfo.energyMeasurementUnit == 1) {
            return "mWh"
        }
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
        id: chargertype
        anchors{ horizontalCenter: leveltext.horizontalCenter; top: batStat.bottom}
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
                chargeState.text = "Not Charging"
            }
            if(batinfo.chargingState == 1) {
                chargeState.text = "Charging"
            }
        }
    }
/////////////////////////

    Particles {
        id: floorParticles
        anchors { horizontalCenter: screen.horizontalCenter; }
        y: screen.height
        width: 1
        height: 1
        source: "images/blueStar.png"
        lifeSpan: 1000
        count: batlevel
        angle: 270
        angleDeviation: 45
        velocity: 50
        velocityDeviation: 60
        ParticleMotionGravity {
            yattractor: 1000
            xattractor: 0
            acceleration: 5
        }
    }

    function particleState() {
        if(img.state == "Battery") {
            particles.burst(50,200);
        }
    }


    Image {
        id: img;
        source: "images/blueStone.png"
        smooth: true
        width: batinfo.battlevel; height: batinfo.battlevel;
        anchors {
            horizontalCenter: screen.horizontalCenter;
        }
        y: screen.height - img.height;
        Particles {
            id: particles
            width:1; height:1; anchors.centerIn: parent;
            emissionRate: 0;
            lifeSpan: 700; lifeSpanDeviation: 300;
            angle: 0; angleDeviation: 360;
            velocity: 100; velocityDeviation:30;
            source:"images/blueStar.png";
        }

        states: [
        State {
            name: "WallPower"
            when: deviceinfo.currentPowerState == 2
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges {
                target: img; opacity: 1; source : "images/blueStone.png";
                anchors.horizontalCenter: undefined
                y: 0;  x: (screen.width / 2) - (img.width / 2)
            }
            PropertyChanges { target: floorParticles; count:0 }

        },
        State {
            name: "Charging"
            when: deviceinfo.currentPowerState == 3
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; y:screen.height
            }
            PropertyChanges {
                target: img; opacity: 1; source : "images/yellowStone.png";
                anchors.horizontalCenter: parent.horizontalCenter;
            }
            PropertyChanges { target: floorParticles; count:0 }
        },

        State {
            name: "Battery"
            when: deviceinfo.currentPowerState == 1
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges {
                target: img; source : "images/redStone.png";
                anchors.horizontalCenter: parent.horizontalCenter;
            }
            PropertyChanges { target: floorParticles; count: batlevel }

        },
        State {
            name: "levelchange"
            PropertyChanges {
                target: yAnim
                running: false;
            }

            PropertyChanges {
                target: bubblebounceanim
                from: screen.height
                to: screen.height - (screen.height * (batlevel / 100 ))
            }
            PropertyChanges {
                target: yAnim
                running: true;
            }
        }
        ]


        transitions: [
        Transition {
            from: "*"
            to: "WallPower"
            NumberAnimation{ property: "y"; to: 0; duration: 750; easing.type: Easing.InOutQuad; }
        },
        Transition {
            from: "WallPower"
            to: "*"
            NumberAnimation{ property: "y"; to: screen.height; duration: 2000; easing.type: Easing.InOutQuad; }
        }
        ]

        SequentialAnimation on y {
            id: yAnim
            loops: Animation.Infinite
            running: img.state != "WallPower"
            NumberAnimation {
                id: bubblebounceanim;
                from: screen.height; to: screen.height - (screen.height * (batlevel / 100 ))
                easing.type: Easing.OutBounce; duration: speed
            }
            ScriptAction { script: particleState() }
            PauseAnimation { duration: 750 }
        }

        SequentialAnimation on x {
            running: img.state == "WallPower"
            loops: Animation.Infinite
            id: xanim
            NumberAnimation { target: img; property: "x"; to: screen.width - img.width; duration: 1500;
                easing.type: Easing.InOutQuad;  }
            NumberAnimation { target: img; property: "x"; to: 0; duration: 1500;
                easing.type: Easing.InOutQuad;}
        }
    }



    /////////////////////////////////
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
