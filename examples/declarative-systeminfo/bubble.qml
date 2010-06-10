import Qt 4.7
import QtMobility.systeminfo 1.0
import Qt.labs.particles 1.0



Rectangle{
    id: screen
    width: 500;
    height: 300;
    color: "#343434"

    property int speed: 1000;

    SystemDeviceInfo {
        id: deviceinfo;

//        property int batlevel :  deviceinfo.batteryLevel;

        function doPowerStateChange(state) {
            console.log("power state changed: "+ state)
            if(deviceinfo.currentPowerState == 1) {
                img.state = 'Battery';
            }
            if(deviceinfo.currentPowerState == 2) {

                img.state = 'WallPower';
            }
            if(deviceinfo.currentPowerState == 3) {
                img.state = 'Charging'
            }
        }

        function doBatteryStatusChange(status) {
console.log("battery status changed: "+status)
            if(status == 4) {
                img.state = 'Charging';
            }
            if(status == 3) {
                img.state = 'Charging';
            }
            if(status == 2) {
                img.state = 'Battery';
            }
        }

        function doBatteryLevelChange(level) {
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
            }
            else if(level > 10) {
                speed = 3500;
            } else if(level < 11) {
                speed = 4000;
            } else {
                speed = 4500;
            }
             floorParticles.burst(level);
        }

        onBatteryStatusChanged : doBatteryStatusChange(status);
        onPowerStateChanged : doPowerStateChange(state);
        onBatteryLevelChanged: doBatteryLevelChange(level)
    }

    function getPowerState(x) {
        state = deviceinfo.currentPowerState;
        if ( state == 1) {
            return "On Battery : " + deviceinfo.batteryLevel +"%"
        }
        if ( state == 2 ) {
            return "Wall Power : " + deviceinfo.batteryLevel +"%"
        }
        if ( state == 3) {
            return "Charging : " + deviceinfo.batteryLevel +"%"
        }
        return ""
    }

    Text {
        id: manu
        y: 3
        text: deviceinfo.manufacturer + " "+ deviceinfo.model+ " "+ deviceinfo.productName
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
        anchors { horizontalCenter: parent.horizontalCenter; }
             y: screen.height
             width: 1
             height: 1
             source: "images/blueStar.png"
             lifeSpan: 1000
             count: deviceinfo.batteryLevel
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
        anchors { horizontalCenter: parent.horizontalCenter; }
        y: screen.height;
       // MouseArea { id: mousearea; anchors.fill: parent; onClicked: test()/*img.state = 'Battery'*/ }

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
            //  when: mousearea.pressed == false
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 1; source : "images/blueStone.png";}
            PropertyChanges { target: floorParticles; count:0 }
        },
        State {
            name: "Charging"
            //  when: mousearea.pressed == false
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; opacity: 1; source : "images/yellowStone.png";}
            PropertyChanges { target: floorParticles; count:0 }
        },

        State {

            name: "Battery"
            //    when: mousearea.pressed == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; source : "images/redStone.png"; /*opacity: 0; */}
            PropertyChanges { target: floorParticles; count:deviceinfo.batteryLevel }
        }
        ]

        SequentialAnimation on y {
            loops: Animation.Infinite

 //            PropertyAction { target: img; property: "opacity"; value: 1 }

            NumberAnimation {
                from: img.y; to: screen.height - (screen.height * (deviceinfo.batteryLevel / 100 ))-img.height
                easing.type: Easing.InOutQuart; duration: speed/* Math.round(Math.abs(Math.cos(deviceinfo.batteryLevel))* 5000 )*/;
            }

            ScriptAction { script: particleState() }
            PauseAnimation { duration: 750 }
        }
    }
}
