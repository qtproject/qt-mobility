import Qt 4.6
Item {
    id: DialScreen
    signal dial
    signal hangup
    property string dialString

    width: contents.width
    height: contents.height

    Rect {
        id: dialStringLabel
        height: 20; width: dialGrid.width
        color: "white"; radius: 5
        pen.width: 3
        pen.color: "black"
        anchors.top: parent.top
        anchors.left: parent.left
        Text { text: DialScreen.dialString; anchors.centeredIn: parent; }
    }
    GridLayout {
        id: dialGrid
        anchors.top: dialStringLabel.bottom
        anchors.left: parent.left
        anchors.topMargin: 5
        columns: 3
        spacing: 5
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
        width: 50;// radius: 5; //color: "crimson"
        hoverColor: "red"; color: "crimson"
        //pen.width: 3; pen.color: "black"
        /*Text {
            text: "Hangup"
            anchors.centeredIn: parent
            transformOrigin: "Center"
            //rotation: 90
        }*/
        Image {
            anchors.centeredIn: parent
            source: "hangup.png"
            transformOrigin: "Center"
            //scale: 1.5
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
            source: "call.png"
            transformOrigin: "Center"
            //scale: 1.5
        }
    }
}
