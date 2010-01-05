import Qt 4.6

//Implementation of the DialButton control.
Item {
    id: DialButton
    width: 50
    height: 50
    property alias buttonText: InnerText.text;
    property alias color: RectangleButton.color
    property color hoverColor: "lightsteelblue"
    property color pressColor: "slategray"
    signal clicked

    Rectangle {
        id: RectangleButton
        anchors.fill: parent
        radius: 5
        color: "steelblue"
        border.width: 3
        border.color: "black"

        Text {
            id: InnerText
            font.pointSize: 20
            anchors.centerIn: parent
        }
    }
   
    states: [
        State {
            name: "Hovering"
            PropertyChanges {
                target: RectangleButton
                color: hoverColor
            }
        },
        State {
            name: "Pressed"
            PropertyChanges {
                target: RectangleButton
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
        anchors.fill: DialButton
        onEntered: { DialButton.state='Hovering'}
        onExited: { DialButton.state=''}
        onClicked: { DialButton.clicked();}
        onPressed: { DialButton.state="Pressed" }
        onReleased: {
            if (containsMouse)
            DialButton.state="Hovering";
            else
            DialButton.state="";
        }
    }
}
