import Qt 4.6
Item {
    id: DialScreen
    signal dial
    signal hangup
    property alias dialString : dialDisplay.dialString

    Item {
        id: dialDisplay
        property string dialString
        //color: "sienna"
        anchors.fill: parent
        Rect {
            id: dialStringLabel
            height: 20; width: dialGrid.width
            color: "white"; radius: 5
            pen.width: 3
            pen.color: "black"
            anchors.top: parent.top
            anchors.left: parent.left
            Text { text: dialDisplay.dialString; anchors.centeredIn: parent; }
        }
        GridLayout {
            id: dialGrid
            anchors.top: dialStringLabel.bottom
            anchors.left: parent.left
            anchors.topMargin: 5
            columns: 3
            spacing: 5
            DialButton { buttonText: "1"; onClicked: { dialDisplay.dialString += "1";} }
            DialButton { buttonText: "2"; onClicked: { dialDisplay.dialString += "2";} }
            DialButton { buttonText: "3"; onClicked: { dialDisplay.dialString += "3";} }
            DialButton { buttonText: "4"; onClicked: { dialDisplay.dialString += "4";} }
            DialButton { buttonText: "5"; onClicked: { dialDisplay.dialString += "5";} }
            DialButton { buttonText: "6"; onClicked: { dialDisplay.dialString += "6";} }
            DialButton { buttonText: "7"; onClicked: { dialDisplay.dialString += "7";} }
            DialButton { buttonText: "8"; onClicked: { dialDisplay.dialString += "8";} }
            DialButton { buttonText: "9"; onClicked: { dialDisplay.dialString += "9";} }
            DialButton { buttonText: "*"; onClicked: { dialDisplay.dialString += "*";} }
            DialButton { buttonText: "0"; onClicked: { dialDisplay.dialString += "0";} }
            DialButton { buttonText: "#"; onClicked: { dialDisplay.dialString += "#";} }
        }
        DialButton {
            id: hangup
            anchors.top: dialGrid.top; anchors.left: dialGrid.right
            anchors.leftMargin: 5
            height: { (dialGrid.height / 2) - 2 }
            width: 50;// radius: 5; //color: "crimson"
            hoverColor: "red"; color: "crimson"
            //pen.width: 3; pen.color: "black"
            Text {
                text: "Hangup"
                anchors.centeredIn: parent
                transformOrigin: "Center"
                //rotation: 90
            }
        }
        DialButton {
            id: connect
            width: 50; height: {(dialGrid.height/2) -2}
            anchors.top: hangup.bottom; anchors.left: dialGrid.right
            anchors.leftMargin: 5; anchors.topMargin: 4
            color: "mediumseagreen"
            hoverColor: "lightgreen"
            Image {
                anchors.centeredIn: parent
                source: "call.svg"
                transformOrigin: "Center"
                //scale: 1.5
            }
        }
    }
}
