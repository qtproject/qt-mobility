import Qt 4.7
import QtMobility.systeminfo 1.1
Rectangle {
    width: 360
    height: 360
    BatteryInfo {
        id: batinfo;
    }
    Text {
        id: level
        anchors.centerIn: parent
        text:   "Level "+batinfo.remainingCapacityPercent
    }
    Text {
        id: voltage
        anchors{ horizontalCenter: level.horizontalCenter; top: level.bottom}
        text: "Voltage: "+ batinfo.voltage
    }
    Text {
        id: nomCap
        anchors{ horizontalCenter: level.horizontalCenter; top: voltage.bottom}
        text: "Nominal Capacity: "+ batinfo.nominalCapacity
    }
    Text {
        id: remCap
        anchors{ horizontalCenter: level.horizontalCenter; top: nomCap.bottom}
        text: "Remaining Capacity: "+ batinfo.remainingCapacity
    }
    Text {
        id: chargeTime
        anchors{ horizontalCenter: level.horizontalCenter; top: remCap.bottom}
        text: "Charge time: "+ batinfo.remainingChargingTime;

    }
    Text {
        id: curFLow
        anchors{ horizontalCenter: level.horizontalCenter; top: chargeTime.bottom}
        text: "Current Energy: "+ batinfo.currentFlow
    }
    Text {
        id: batStat
        anchors{ horizontalCenter: level.horizontalCenter; top: curFLow.bottom}
        text: "Battery Status: "+ batinfo.batteryStatus
    }

    Text {
        id: energyUnit
        anchors{ horizontalCenter: level.horizontalCenter; top: batStat.bottom}
        text: "Energy unit: "+ batinfo.energyMeasurementUnit
    }



    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
