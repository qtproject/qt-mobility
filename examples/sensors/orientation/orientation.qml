import Qt 4.6

Rectangle {
    width: 800
    height: 422
    color: "black"

    OrientationSensor {
        id: orientation

        onReadingChanged: {
            console.log(orientation.reading.orientation);
            if (orientation.reading.orientation == 3)
                content.state = "Portrait";
            else if (orientation.reading.orientation == 2)
                content.state = "";
        }
    }

    Timer {
        interval: 0; running: true; repeat: false
        onTriggered: orientation.start()
    }

    Item {
        id: content
        transformOrigin: Item.Center
        anchors.centerIn: parent
        Text {
            text: "hello world"
            color: "white"
            font.pointSize: 24
            anchors.centerIn: parent
        }

        states: State {
            name: "Portrait"
            PropertyChanges { target: content; rotation: -90 }
        }
        transitions: Transition {
            NumberAnimation { matchProperties: "rotation"; easing: "InOutQuad"; duration: 400 }
        }
    }


}
