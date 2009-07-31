import Qt 4.6
Rect {
    id: Page
    width: 500
    height: 250
    color: "sienna"
    Rect {
        height: contents.height + 10; width: contents.width
        anchors.top: parent.top; anchors.left: parent.left; anchors.right: screen.left
        anchors.topMargin: 5; anchors.leftMargin: 5; anchors.rightMargin: 5
        radius: 5; color: "steelblue"
        pen.color: "black"; pen.width: 3
        gradient: Gradient {
                    GradientStop {position: 0.0; color: "lightsteelblue"}
                    GradientStop {position: 1.0; color: "steelblue"}
                }
        Text {
            id: label1
            text: "<b>Select dial service:</b>"
            height: 20; width: 200
            anchors.top: parent.top; anchors.left: parent.left
            anchors.topMargin: 5; anchors.leftMargin: 5
        }
        Rect {
            pen.color: "black"; pen.width: 3
            width: contents.width; height: contents.height
            anchors.top: label1.bottom; anchors.left: parent.left;
            anchors.topMargin: 5; anchors.leftMargin: 5; anchors.rightMargin: 5
            Component {
                id: Delegate
                Rect {
                    id: Root
                    MouseRegion {
                        anchors.fill: parent
                        onClicked: Root.ListView.view.currentIndex = index
                    }
                    pen.color: "black"; pen.width: 1
                    width: parent.width
                    height: 40
                    opacity: 0.6
                    Text { text: " <b>Service:</b> "+ modelData.serviceName + "  ("+modelData.version+")";
                        anchors.top: parent.top; id: sname; anchors.topMargin: 5; anchors.leftMargin: 3; anchors.left: parent.left
                    }
                    Text { text: " <b>Interface:</b> " + modelData.interfaceName;
                        anchors.top: sname.bottom; anchors.topMargin: 2; anchors.leftMargin: 3; anchors.left: parent.left
                    }
                }
            }
            Component {
                id: Highlight
                Rect {
                    color : "lightsteelblue"
                    width: parent.width
                    pen.color: "black"; pen.width: 2
                    height: 30
                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "steelblue"}
                        GradientStop {position: 0.5; color: "lightsteelblue"}
                        GradientStop {position: 1.0; color: "steelblue"}
                    }
                }
            }
            ListView {
                model: services
                height: 100; width: 260
                opacity: 1
                anchors.topMargin: 5; anchors.leftMargin: 5; anchors.rightMargin: 5
                delegate: Delegate
                highlight: Highlight
                clip: true

            }
        }
    }
    DialScreen {
        id: screen
        anchors.topMargin: 5; anchors.leftMargin: 5
        anchors.rightMargin: 5;
        //anchors.left: DialerSelection.right
        anchors.right: parent.right
        anchors.top: parent.top
    }



}
