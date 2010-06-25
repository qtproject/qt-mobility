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
import "content"

Rectangle {
    id: screen
    width: 640
    height: 480
    color: "#343434"


    SystemDeviceInfo {
        id: deviceinfo;
    }

    SystemNetworkInfo {
        id: netinfo;
    }

    property int maxvalue: deviceinfo.batteryLevel

    Connections {
        target: deviceinfo;
        onCurrentPowerStateChanged: { getPowerState(x);}
        onBatteryLevelChanged: { console.log("battery level changed"); }
    }

    function getPowerState(x) {
       state = deviceinfo.currentPowerState;
        if ( state == 1) {
            return "On Battery : " + maxvalue +"%"
        }
        if ( state == 2 ) {
            return "Wall Power : " + maxvalue +"%"
        }
        if ( state == 3) {
            return "Charging : " + maxvalue +"%"
        }
        return ""
    }

//    SystemNetworkInfo {
//         id: netinfo
//     }
//    SystemStorageInfo {
//         id: storageinfo
//     }
//    SystemStorageInfo {
//        id: storInfo
//    }

    Text {
        id: manu
         y: 3
         text: deviceinfo.manufacturer + " "+ deviceinfo.model
         color: "white";
     }

     property string batterystatus
     property string powerstate
     property int minvalue: 0
     property int loopsval: Animation.Infinite

     function getBatteryStatus(status) {
         switch(status) {
         case 1: batterystatus = "Critical"; break;
         case 2: batterystatus = "Very Low"; break;
         case 3: batterystatus = "Low"; break;
         case 4: batterystatus = "Normal"; break;
         }
         return batterystatus
     }

     Text {
         id: power
         text: getPowerState();
         anchors.top: manu.bottom
         color: "white";
     }

     Text {
         id:batterystat
         color: "white";
         text: getBatteryStatus(deviceinfo.batteryStatus)
         anchors.top: power.bottom
//           Binding { target: deviceinfo; property: "currentPowerState"; value: getPowerState(); }
     }

     function doBatteryBar() {
         console.log("batterybar");
//         console.log(deviceinfo.currentPowerState)

         if(deviceinfo.currentPowerState == 1) {
             batteryLevelProgressBar.state = 'battery';
         }
         if(deviceinfo.currentPowerState == 2) {
           //  maxvalue = deviceinfo.batteryLevel;
             loopsval = 2;
             batteryLevelProgressBar.state = 'wallpower';
         }
         if(deviceinfo.currentPowerState == 3) {
             batteryLevelProgressBar.state = 'charging'
         }

     }

//     ProgressBar {
//         id: batteryLevelProgressBar
//         property int maxval: maxvalue
//         property int loops : loopsval;


//         NumberAnimation on value {id: glob; duration: 2500; from: minvalue; to: maxvalue; loops: loopsval;}
//         ColorAnimation on color { duration: 2500; from: "lightsteelblue"; to: "thistle"; loops: Animation.Infinite }
//         ColorAnimation on secondColor { duration: 2500;from: "steelblue"; to: "#CD96CD"; loops: Animation.Infinite }
//         anchors {top: batterystat.bottom; horizontalCenter: parent.horizontalCenter }

//         states: [
//         State {
//             name: "battery";
//             PropertyChanges { target: batteryLevelProgressBar;}
//         },

//         State {
//             name: "wallpower";
//             PropertyChanges { target: batteryLevelProgressBar;  loops: 4;/* maxval: deviceinfo.batteryLevel;*/ }
//             PropertyChanges { target: glob; loops: 4; }
//         },

//         State {
//             name: "charging";
//             PropertyChanges { target: batteryLevelProgressBar; /*easing.type:Easing.OutBounce; loops: 3; maxvalue: deviceinfo.batteryLevel;  minvalue: 0;*/ }
//         }
//         ]
//     }

//     transitions: [
//     Transition {
//         from: "*"
//         to: "wallpower"
//         NumberAnimation{target: glob;  from: minvalue ; to: maxvalue; easing.type:Easing.OutBounce;}
//     },
//     Transition {
//         from: "*"
//         to: "battery"
//         NumberAnimation{ target: glob; from: maxvalue; to: minvalue; easing.type:Easing.InBounce; loops: Animation.Infinite}
//     }
//     ]


     Text {
         id: netmode
         //text: getNetworkInfo();
         anchors.top: batterystat.bottom
     }
     Component.onCompleted: getNetworkInfo();

     property int curmode;
     property int netStatus: netinfo.networkStatus( wlan );

     function getNetworkInfo() {
         curmode = netinfo.currentMode;
         console.log("current network mode "+ curmode);

         console.log("current network status "+ netStatus);

         console.log("mac address is: "+ netinfo.macAddress(4));

         console.log("net status " + netinfo.networkStatus.netinfo.WlanMode);
         console.debug("input "+ deviceinfo.inputMethodType);

         if(curmode == 5) {
             return "Ethernet"
         }

         if(curmode == 4) {
             return "Wlan"
         }

     }
 }
