import Qt 4.6

Rectangle {
    id: Page
    width: 500
    height: 250
    color: "sienna"

    Rectangle {
        id: ServiceSelectionBox
        property variant service: 0

        height: childrenRect.height + 10; width: childrenRect.width
        anchors.top: parent.top; anchors.left: parent.left; anchors.right: screen.left
        anchors.topMargin: 5; anchors.leftMargin: 5; anchors.rightMargin: 5
        radius: 5; color: "steelblue"
        border.color: "black"; border.width: 3
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
        Rectangle {
            border.color: "black"; border.width: 3
            width: childrenRect.width; height: childrenRect.height
            anchors.top: label1.bottom; anchors.left: parent.left;
            anchors.topMargin: 5; anchors.leftMargin: 5; anchors.rightMargin: 5
            Component {
                id: Delegate
                Rectangle {
                    id: Root
                    MouseRegion {
                        anchors.fill: parent
                        onClicked: {
                            Root.ListView.view.currentIndex = index;
                            ServiceSelectionBox.service = modelData;
                        }
                    }
                    border.color: "black"; border.width: 1
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
                Rectangle {
                    color : "lightsteelblue"
                    width: parent.width
                    border.color: "black"; border.width: 2
                    height: 30
                    gradient: Gradient {
                        GradientStop {position: 0.0; color: "steelblue"}
                        GradientStop {position: 0.5; color: "lightsteelblue"}
                        GradientStop {position: 1.0; color: "steelblue"}
                    }
                }
            }
            ListView {
                id: myList
                model: services
                height: 100; width: 260
                opacity: 1
                anchors.topMargin: 5; anchors.leftMargin: 5; anchors.rightMargin: 5
                delegate: Delegate
                highlight: Highlight
                currentIndex: 0
                clip: true
            }
        }
    }
    Timer {
        id: cleanupTimer
        interval: 2000; running: false; repeat: false
        onTriggered: status.text = ""
    }

    DialScreen {
        property bool indial : false
        property var currentDialer: 0;
        property int dialState: 0
        id: screen
        anchors.topMargin: 5; anchors.leftMargin: 5
        anchors.rightMargin: 5;
        anchors.right: parent.right
        anchors.top: parent.top
        onDial: {
            if (indial == false) {
                indial = true;
                status.text = "Dialing " + numberToDial +"...";
                if (ServiceSelectionBox.service != 0) {
                    var o = ServiceSelectionBox.service.serviceObject();
                    screen.currentDialer = o;
                    o.dialNumber(numberToDial);
                }
            }
        }
        onHangup: {
            if (indial) {
                if (screen.currentDialer != 0) {
                        screen.currentDialer.hangup();
                }
            }
        }

        Script {
            function OnDialerStateChanged(newState) {

            }
        }

        Connection {
            sender: screen.currentDialer
            signal: "stateChanged()"
            script: {
                dialState = screen.currentDialer.state;
                if (dialState == 1) {
                    status.text += "\nRinging";
                } else if (dialState == 2) {
                    status.text += "\nConnected";
                } else if (dialState == 0) {
                    status.text += "\nConnection terminated";
                    indial = false;
                    cleanupTimer.running = true;
                } else if (dialState == 3) {
                    status.text += "\nPhone already engaged";
                }
            }
        }
    }

    Text {
        anchors.topMargin: 5; anchors.leftMargin: 5
        anchors.rightMargin: 5;
        anchors.left: parent.left
        anchors.top: ServiceSelectionBox.bottom
        id: status
    }
}
