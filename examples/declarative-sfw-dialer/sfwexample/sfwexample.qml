import Qt 4.6

//Layout of the mainPage
//----------------------------------------------  ____ mainPage
//| ------------------- ---------------------- | /
//| | serviceList     | | dialScreen         | |/
//| |                 | |                    | |
//| |                 | |                    | |
//| |                 | |                    | |
//| ------------------- |                    | |
//| ------------------- |                    | |
//| | serviceDetails  | |                    | |
//| ------------------- |                    | |
//|                     |                    | |
//|                     |                    | |
//|                     |                    | |
//|                     |                    | |
//| ------------------- |                    | |
//| | status          | |                    | |
//| ------------------- ---------------------- |
//----------------------------------------------

Rectangle {
    id: mainPage
    width: 500
    height: 250
    color: "white"

    ServiceList {
        id: serviceList
        height: childrenRect.height + 10
        width: childrenRect.width
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: dialScreen.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        radius: 5
        color: "steelblue"
        border.color: "black"
        border.width: 3
        gradient:     
            Gradient {
                GradientStop {
                    position: 0.0
                    color: "lightsteelblue"
                }
                
                GradientStop {
                    position: 1.0
                    color: "steelblue"
                }
            }
        onServiceSelected: { ServiceSelected(); }
    }

    Script {
        function ServiceSelected()
        {
            serviceDetails.text = "Selected dial service:" + "\n   " + 
                                   serviceList.dialService.serviceName + 
                                   "\n   (" + serviceList.dialService.version + ")";
        }
    }
    
    Text {
        id: serviceDetails
        text: "Selected dial service:"
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5;
        anchors.left: parent.left
        anchors.top: serviceList.bottom
    }
    
    Text {
        id: status
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: -40
        anchors.leftMargin: 5
    }
    
    Timer {
        id: clearStatusTimer
        interval: 2000
        running: false
        repeat: false
        onTriggered: {
            status.text = ""
        }
    }

    //! [0]
    DialScreen {
        id: dialScreen
        property bool activeCall : false
        property var currentDialer: 0;
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.right: parent.right
        anchors.top: parent.top
        onDial: {
            if (activeCall == false) {
                if (serviceList.dialService != 0) {
                    var o = serviceList.dialService.serviceObject();
                    status.text = "Dialing " + numberToDial +"...";
                    dialScreen.currentDialer = o;
                    o.dialNumber(numberToDial);
                    activeCall = true;
                }
            }
        }
        onHangup: {
            if (activeCall) {
                if (dialScreen.currentDialer != 0) {
                    dialScreen.currentDialer.hangup();
                }
                status.text = "Hang up";
            }
        }
    }
    //! [0]

    //! [1]
    Connections {
        target: dialScreen
        signal: "stateChanged()"
        script: { 
            if (dialScreen.currentDialer.state == 1) {
                status.text += "\nRinging";
            } 
            else if (dialScreen.currentDialer.state == 2) {
                status.text += "\nConnected";
            } 
            else if (dialScreen.currentDialer.state == 0) {
                status.text += "\nConnection terminated";
                dialScreen.activeCall = false;
                clearStatusTimer.running = true;
            } 
            else if (dialScreen.currentDialer.state == 3) {
                status.text += "\nPhone already engaged";
            }
        }
    }
    //! [1]
}
