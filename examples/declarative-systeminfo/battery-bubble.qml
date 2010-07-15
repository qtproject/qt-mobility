/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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
***********************************/

import Qt 4.7
import QtMobility.systeminfo 1.0
import Qt.labs.particles 1.0

Rectangle{
    id: screen
    width: 500;
    height: 300;
    color: "#343434"

    property int speed: 1000;

    DeviceInfo {
        id: deviceinfo;
        //        onBatteryStatusChanged : doBatteryStatusChange(status);
        onPowerStateChanged : getPowerState(powerState);
        onBatteryLevelChanged: doBatteryLevelChange(level)
        property int battlevel: batteryLevel;

    }

    property alias batlevel: deviceinfo.battlevel;
    property alias curPowerState: deviceinfo.currentPowerState;

    property string oldstate;

    function doBatteryLevelChange(level) {
        //  console.log("doBatteryLevel: "+level + " " + deviceinfo.batlevel)
        if(level > 90) {
            speed = 1000;
        } else if(level > 70) {
            speed = 1500;
        } else if(level > 60) {
            speed = 2000;
        } else if(level > 50) {
            speed = 2500;
        } else if(level > 40) {
            speed = 3000;
        } else if(level > 10) {
            speed = 3500;
        } else if(level < 11) {
            speed = 4000;
        }
        floorParticles.burst(level);
        batlevel = level;
        oldstate = img.state;
        img.state = "levelchange"
        img.state = oldstate;
    }

    function getPowerState(powerState) {
        console.log("get power state: "+powerState)

        if(powerState == DeviceInfo.UnknownPower) {
            console.log("get power state: "+powerState)
        }

        if(powerState == DeviceInfo.BatteryPower) {
            console.log("battery power state: "+powerState)
        }

        if(powerState == DeviceInfo.WallPower) {
            console.log("wall state: "+powerState)
        }

        if(powerState == DeviceInfo.WallPowerChargingBattery) {
            console.log("charging power state: "+powerState)
        }

        // state = curPowerState;
        if ( curPowerState == 1) {
            return "On Battery : " + batlevel +"%"
        }
        if ( curPowerState == 2 ) {
            return "Wall Power : " + batlevel +"%"
        }
        if ( curPowerState == 3) {
            return "Charging : " + batlevel +"%"
        }
        return ""
    }

    property alias manufacturer: deviceinfo.manufacturer
    property alias model: deviceinfo.model
    property alias product: deviceinfo.productName

    Text {
        id: manu
        y: 3
        text: manufacturer + " "+ model+ " "+ product
        color: "white";
    }

    Text {
        id: power
        text: getPowerState();
        anchors.top: manu.bottom
        color: "white";
    }

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
            //            NumberAnimation{ property: "x"; to: screen.width / 2 - img.width / 2; duration: 750; easing.type: Easing.InOutQuad; }
            //            PropertyAction{ target: bubblebounceanim; properties: "to"; value: screen.height - (screen.height * (batlevel / 100 )); }
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
}
