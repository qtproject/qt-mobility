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
        onBatteryStatusChanged : img.state = 'Battery';
    }

    //getBattStat
    Image {
        id: img;
        source: "images/blueStone.png"
        property int randomX: 450;//Math.random() * 500 - 100
        property int randomY: 300;//Math.random() * 200 + 140
        x: randomX;
        y:randomY;
        MouseArea { id: mousearea; anchors.fill: parent; onClicked: img.state = 'Battery' }


        Particles {
            id: particles
            width:1; height:1; anchors.centerIn: parent;
            emissionRate: 0;//deviceinfo.batteryLevel;
            lifeSpan: 700; lifeSpanDeviation: 300;
            angle: 0; angleDeviation: 360;
            velocity: 100; velocityDeviation:30;
            source:"images/blueStar.png";
        }

        states: [
        State {
            name: "Charging"
          //  when: mousearea.pressed == false
            PropertyChanges { target: img; opacity: 1; source : "images/blueStone.png";}
        },

        State {
            name: "Battery"
        //    when: mousearea.pressed == true
            StateChangeScript { script: particles.burst(50); }
            PropertyChanges { target: img; source : "images/redStone.png"; }
        }
        ]

        SequentialAnimation on y {
            loops: Animation.Infinite

            NumberAnimation {
                from: img.y; to: -img.height
                easing.type: Easing.InOutQuart; duration: { Math.abs(Math.cos(deviceinfo.batteryLevel)) * 5000; }
            }

            PauseAnimation { duration: 750 }
        }
        //      }
    }

//    property int batcharge: deviceinfo.batteryLevel;
//    Timer{ id: tick; repeat: true; running: true; interval: 10; /*onTriggered: Script.tick();*/ }
//    Component.onCompleted: Script.initBalls();
}
