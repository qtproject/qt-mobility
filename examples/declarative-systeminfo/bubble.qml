import Qt 4.7
import QtMobility.systeminfo 1.0
import Qt.labs.particles 1.0



Rectangle{
    id: screen
    width: 500;
    height: 300;
    color: "#343434"

    SystemDeviceInfo {
        id: deviceinfo;
        property int batlevel :  deviceinfo.batteryLevel;

        function doPowerStateChange() {
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

        function doBatteryStatusChange() {

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
        onBatteryStatusChanged : doBatteryStatusChange();
        onPowerStateChanged : doPowerStateChange();
    }


    Particles {
        anchors { horizontalCenter: parent.horizontalCenter; }
             y: screen.height
             width: 1
             height: 1
             source: "images/blueStar.png"
             lifeSpan: 50000
             count: deviceinfo.batlevel
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

        function runParticleBurst() {
            particles.burst(50);
        }

        states: [
        State {
            name: "WallPower"
            //  when: mousearea.pressed == false
            PropertyChanges { target: img; opacity: 1; source : "images/blueStone.png";}
        },
        State {
            name: "Charging"
            //  when: mousearea.pressed == false
            PropertyChanges { target: img; opacity: 1; source : "images/yellowStone.png";}
        },

        State {
            name: "Battery"
            //    when: mousearea.pressed == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; source : "images/redStone.png"; /*opacity: 0; */}
        }
        ]


        transitions: [
        Transition {
            from: "*"
            to: "WallPower"
//   NumberAnimation{target: glob;  from: minvalue ; to: maxvalue; easing.type:Easing.OutBounce;}
        },
        Transition {
            from: "*"
            to: "Battery"
        }
        ]

        Behavior on opacity {
            NumberAnimation { properties:"opacity";duration: 500 }
        }

        SequentialAnimation on y {
            loops: Animation.Infinite

 //            PropertyAction { target: img; property: "opacity"; value: 1 }

            NumberAnimation {
                from: img.y; to: screen.height - (screen.height * (deviceinfo.batteryLevel / 100 ))-img.height
                easing.type: Easing.InOutQuart; duration: Math.round(Math.abs(Math.cos(deviceinfo.batlevel))* 5000 );
            }

            PauseAnimation { duration: 750 }
        }
    }
}
