import Qt 4.6
Rect {
    id: Page
    width: 500
    height: 250
    color: "sienna"

    Rect {
        id: DialerSelection
        anchors.topMargin: 5
        width: 200
        height: 240
        radius: 5; pen.width: 3; pen.color: "black"
        anchors.top: parent.top; anchors.left: parent.left

        Text {
            id: label1
            text: "Available interfaces"
            anchors.top: parent.top; anchors.left: parent.left
            anchors.topMargin: 5; anchors.leftMargin: 5
        }

    }

    DialScreen {
        id: screen
        anchors.topMargin: 5; anchors.leftMargin: 5
        anchors.left: DialerSelection.right
        //anchors.right: parent.right
        anchors.top: parent.top
    }



}
