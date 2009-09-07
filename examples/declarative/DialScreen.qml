import Qt 4.6
Item {
    id: DialScreen
    signal dial(string numberToDial)
    signal hangup
    property string dialString

    width: childrenRect.width
    height: childrenRect.height

    Rectangle {
        id: dialStringLabel
        height: 20; width: dialGrid.width
        color: "white"; radius: 5
        border.width: 3
        border.color: "black"
        anchors.top: parent.top
        anchors.left: parent.left
        Text { text: DialScreen.dialString; anchors.centerIn: parent; }
    }
    Grid {
        id: dialGrid
        anchors.top: dialStringLabel.bottom
        anchors.left: parent.left
        anchors.topMargin: 5
        columns: 3
        spacing: 5
        width: childrenRect.width; height: childrenRect.height
        DialButton { buttonText: "1"; onClicked: { DialScreen.dialString += "1";} }
        DialButton { buttonText: "2"; onClicked: { DialScreen.dialString += "2";} }
        DialButton { buttonText: "3"; onClicked: { DialScreen.dialString += "3";} }
        DialButton { buttonText: "4"; onClicked: { DialScreen.dialString += "4";} }
        DialButton { buttonText: "5"; onClicked: { DialScreen.dialString += "5";} }
        DialButton { buttonText: "6"; onClicked: { DialScreen.dialString += "6";} }
        DialButton { buttonText: "7"; onClicked: { DialScreen.dialString += "7";} }
        DialButton { buttonText: "8"; onClicked: { DialScreen.dialString += "8";} }
        DialButton { buttonText: "9"; onClicked: { DialScreen.dialString += "9";} }
        DialButton { buttonText: "*"; onClicked: { DialScreen.dialString += "*";} }
        DialButton { buttonText: "0"; onClicked: { DialScreen.dialString += "0";} }
        DialButton { buttonText: "#"; onClicked: { DialScreen.dialString += "#";} }
    }
    DialButton {
        id: hangup
        anchors.top: dialGrid.top; anchors.left: dialGrid.right
        anchors.leftMargin: 5
        height: { (dialGrid.height / 2) - 2 }
        width: 50;
        hoverColor: "red"; color: "crimson"
        onClicked:  { DialScreen.dialString = ""; DialScreen.hangup() }
        Image {
            anchors.centerIn: parent
            source: "hangup.png"
            transformOrigin: "Center"
        }
    }
    DialButton {
        id: connect
        width: 50; height: {(dialGrid.height/2) -2}
        anchors.top: hangup.bottom; anchors.left: dialGrid.right
        anchors.leftMargin: 5; anchors.topMargin: 4
        color: "mediumseagreen"
        hoverColor: "lightgreen"
        onClicked: {
            if (DialScreen.dialString != "") {
                DialScreen.dial(dialString);
                DialScreen.dialString = ""
            }
        }
        Image {
            anchors.centerIn: parent
            source: "call.png"
            transformOrigin: "Center"
        }
    }
}
