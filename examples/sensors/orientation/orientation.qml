import Qt 4.6

Rectangle {
    width: 800
    height: 422
    color: "black"

    OrientationSensor {
        id: orientation

        onReadingChanged: {
            if (reading.orientation == OrientationReading.LeftUp)
                content.state = "Portrait";
            else if (reading.orientation == OrientationReading.BottomDown)
                content.state = "";
        }
    }
    Component.onCompleted: orientation.start()

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
