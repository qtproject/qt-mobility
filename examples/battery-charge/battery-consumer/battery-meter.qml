import Qt 4.6
//! [4]
import Example 1.0
//! [4]

Rectangle {
    color: "white"

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
        height: 196 * battery.charge / 100
        clip: true
        color: "green"
        //! [1]

        Particles {
            id: Bubbles
            width: parent.width
            anchors.bottom: parent.bottom
            source: "bubble.png"
            emitting: false
            streamIn: true
            count: battery.charge / 5
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
            when: battery.charging
            PropertyChanges {
                target: Bubbles
                emitting: true
            }
        },
        //! [3]
        //! [2]
        State {
            name: "low"
            when: battery.charge < 25 && !battery.charging
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
    BatteryCharge {
        id: battery
        path: "/power/battery"
    }
    //! [0]
}
