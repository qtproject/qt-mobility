import Qt 4.7
//! [4]
import QtMobility.publishsubscribe 1.0
//! [4]
import Qt.labs.particles 1.0
import "content"

Rectangle {
    color: "white"
    width: 100
    height: 230

    Rectangle {
        x: 20
        y: 10
        width: 60
        height: 10
        color: "black"
    }

    Rectangle {
        x: 10
        y: 20
        width: 80
        height: 200
        color: "black"
    }

    Rectangle {
        //! [1]
        id: visualCharge
        x: 12
        y: 22 + 196 - height
        width: 76
        height: 196 * batteryCharge.value / 100
        clip: true
        color: "green"
        //! [1]

        Particles {
            id: bubbles
            width: parent.width
            anchors.bottom: parent.bottom
            source: "content/bubble.png"
            count: 0
            velocity: 30
            velocityDeviation: 10
            angle: -90
            //lifeSpan: parent.height * 1000 / (velocity + velocityDeviation / 2)
            lifeSpan: parent.height * 1000 / (30 + 10 / 2)
        }

        states: [
        //! [3]
        State {
            name: "charging"
            when: batteryCharging.value
            PropertyChanges {
                target: bubbles
                count: batteryCharge.value / 5
                emissionRate: 5
            }
        },
        //! [3]
        //! [2]
        State {
            name: "low"
            when: batteryCharge.value < 25 && !batteryCharging.value
            PropertyChanges {
                target: visualCharge
                color: "red"
            }
        }
        //! [2]
        ]

        transitions: [
        Transition {
            from: "*"
            to: "low"
            reversible: true
            ColorAnimation {
                duration: 200
            }
        }
        ]
    }

    //! [0]
    ValueSpaceSubscriber {
        id: batteryCharge
        path: "/power/battery/charge"
    }
    ValueSpaceSubscriber {
        id: batteryCharging
        path: "/power/battery/charging"
    }
    //! [0]
}
