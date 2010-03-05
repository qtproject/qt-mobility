import Qt 4.6

//Implementation of the dialButton control.
Item {
    id: dialButton
    width: 50
    height: 50
    property alias buttonText: innerText.text;
    property alias color: rectangleButton.color
    property color hoverColor: "lightsteelblue"
    property color pressColor: "slategray"
    signal clicked

    Rectangle {
        id: rectangleButton
        anchors.fill: parent
        radius: 5
        color: "steelblue"
        border.width: 3
        border.color: "black"

        Text {
            id: innerText
            font.pointSize: 20
            anchors.centerIn: parent
        }
    }
   
    states: [
        State {
            name: "Hovering"
            PropertyChanges {
                target: rectangleButton
                color: hoverColor
            }
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: rectangleButton
                color: pressColor
            }
        }
    ]
   
   
    transitions: [
        Transition {
            from: ""; to: "Hovering"
            ColorAnimation { duration: 100 }
        },
        Transition {
            from: "*"; to: "Pressed"
            ColorAnimation { duration: 10 }
        }
    ]
   
    MouseRegion {
        hoverEnabled: true
        anchors.fill: dialButton
        onEntered: { dialButton.state='Hovering'}
        onExited: { dialButton.state=''}
        onClicked: { dialButton.clicked();}
        onPressed: { dialButton.state="Pressed" }
        onReleased: {
            if (containsMouse)
            dialButton.state="Hovering";
            else
            dialButton.state="";
        }
    }
}
