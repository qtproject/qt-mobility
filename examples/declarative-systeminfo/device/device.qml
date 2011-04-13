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
import QtMobility.systeminfo 1.2

Rectangle {
    id: screen
    width: 360
    height: 360

    Text {
        id: textinfo
        text: devinfo.manufacturer + " " + devinfo.productName + " " + devinfo.model
    }
    Text {
        id: textinfo2
        text: devinfo.uniqueDeviceID
        anchors{ top: textinfo.bottom}
    }
    Text {
        id: textinfo2a
        text: "IMEI :" + devinfo.imei
        anchors{ top: textinfo2.bottom}
    }
    Text {
        id: textinfo2b
        text: "IMSI :" + devinfo.imsi
        anchors{ top: textinfo2a.bottom}
    }
    Text {
        id: textinfo2c
        text: "Sim :" + devinfo.simStatus
        anchors{ top: textinfo2b.bottom}
    }
    Text {
        id: textinfo3
        text: devinfo.primaryKeypadLightOn ? "Primary keypad Light on" : "Primary Keypad Light off"
        anchors{ top: textinfo2c.bottom}
    }
    Text {
        id: textinfo4
        text: devinfo.secondaryKeypadLightOn ? "Secondary keypad Light on" : "Secondary Keypad Light off"
        anchors{ top: textinfo3.bottom}
    }
    Text {
        id: textinfo5
        text: devinfo.currentBluetoothPowerState ? "Bluetooth is on" : "Bluetooth is Off"
        anchors{ top: textinfo4.bottom}
    }
    Text {
        id: textinfo6
        text: devinfo.isDeviceLocked ? "Device lock is enabled" : "Device lock is disabled"
        anchors{ top: textinfo5.bottom}
    }
    Text {
        id: textinfo7
        text: devinfo.isWirelessKeyboardConnected ? "Wireless keyboard is connected" : "Wireless keyboard is not connected"
        anchors{ top: textinfo6.bottom}
    }
    Text {
        id: textinfo8
        text: devinfo.isKeyboardFlipped ? "Keyboard is open" : "Keyboard is closed"
        anchors{ top: textinfo7.bottom}
    }
    Text {
        id: textinfo9
        text: "Battery charge: "+ devinfo.batteryLevel +"%"
        anchors{ top: textinfo8.bottom}
    }

    Text {
        id: textinfo10
        anchors{ top: textinfo9.bottom}
    }
    Text {
        id: textinfo11
        anchors{ top: textinfo10.bottom}
    }

    Text {
        id: textinfo12
        text: updateThermalState()
        anchors{ top: textinfo11.bottom}
    }


    DeviceInfo {
        id: devinfo;
        monitorKeyboardFlips: true
        monitorWirelessKeyboardConnects: true
        monitorBluetoothStateChanges: true

        monitorBatteryLevelChanges: true
        monitorPowerStateChanges: true
        monitorBatteryStatusChanges: true
        monitorThermalStateChanges: true

        onBatteryLevelChanged: {
            textinfo9
        }
        onBatteryStatusChanged: {

            if(devinfo.batteryStatus == -1) {
                textinfo10.text = "Battery Status: Unknown"
            }
            if(devinfo.batteryStatus == 0) {
                textinfo10.text = "Battery Status: Empty"
            }
            if(devinfo.batteryStatus == 1) {
                textinfo10.text = "Battery Status: Critical"
            }
            if(devinfo.batteryStatus == 2) {
                textinfo10.text = "Battery Status: Very Low"
            }
            if(devinfo.batteryStatus == 3) {
                textinfo10.text = "Battery Status: Low"
            }
            if(devinfo.batteryStatus == 4) {
                textinfo10.text = "Battery Status: Ok"
            }
            if(devinfo.batteryStatus == 5) {
                textinfo10.text = "Battery Status: Full"
            }
        }

        onPowerStateChanged: {

         if(devinfo.currentPowerState == 0)
             textinfo11.text = "Power State: Unknown power"
         if(devinfo.currentPowerState == 1)
             textinfo11.text = "Power State: Battery power"
         if(devinfo.currentPowerState == 2)
             textinfo11.text = "Power State: Wall power"
         if(devinfo.currentPowerState == 3)
             textinfo11.text = "Power State: Wall power charging"
        }

        onThermalStateChanged: updateThermalState()
    }

    function updateThermalState() {
        state = devinfo.currentThermalState
        if (state == DeviceInfo.UnknownThermal) {
            textinfo12.text = "ThermalState: UnknownThermal";
        } else if (state == DeviceInfo.NormalThermal) {
            textinfo12.text = "ThermalState: NormalThermal";
        } else if (state == DeviceInfo.WarningThermal) {
            textinfo12.text = "ThermalState: WarningThermal";
        } else if (state == DeviceInfo.AlertThermal) {
            textinfo12.text = "ThermalState: AlertThermal";
        } else if (state == DeviceInfo.ErrorThermal) {
            textinfo12.text = "ThermalState: ErrorThermal";
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
