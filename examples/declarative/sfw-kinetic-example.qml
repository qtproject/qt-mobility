import Qt 4.6

//Layout of the MainPage
//----------------------------------------------  ____ MainPage
//| ------------------- ---------------------- | /
//| | ServiceList     | | DialScreen         | |/
//| |                 | |                    | |
//| |                 | |                    | |
//| |                 | |                    | |
//| ------------------- |                    | |
//| ------------------- |                    | |
//| | ServiceDetails  | |                    | |
//| ------------------- |                    | |
//|                     |                    | |
//|                     |                    | |
//|                     |                    | |
//|                     |                    | |
//| ------------------- |                    | |
//| | Status          | |                    | |
//| ------------------- ---------------------- |
//----------------------------------------------

Rectangle {
    id: MainPage
    width: 500
    height: 250
    color: "white"

    ServiceList {
        id: ServiceList
        height: childrenRect.height + 10
        width: childrenRect.width
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: DialScreen.left
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
            ServiceDetails.text = "Selected dial service:" + "\n   " + ServiceList.dialService.serviceName + "\n   (" + ServiceList.dialService.version + ")";
        }
    }
    
    Text {
        id: ServiceDetails
        text: "Selected dial service:"
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5;
        anchors.left: parent.left
        anchors.top: ServiceList.bottom
    }
    
    Text {
        id: Status
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: -40
        anchors.leftMargin: 5
    }
    
    Timer {
        id: ClearStatusTimer
        interval: 2000
        running: false
        repeat: false
        onTriggered: {
            Status.text = ""
        }
    }

    //! [0]
    DialScreen {
        id: DialScreen
        property bool activeCall : false
        property var currentDialer: 0;
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.right: parent.right
        anchors.top: parent.top
        onDial: {
            if (activeCall == false) {
                if (ServiceList.dialService != 0) {
                    var o = ServiceList.dialService.serviceObject();
                    Status.text = "Dialing " + numberToDial +"...";
                    DialScreen.currentDialer = o;
                    o.dialNumber(numberToDial);
                    activeCall = true;
                }
            }
        }
        onHangup: {
            if (activeCall) {
                if (DialScreen.currentDialer != 0) {
                    DialScreen.currentDialer.hangup();
                }
                Status.text = "Hang up";
            }
        }
    }
    //! [0]

    //! [1]
    Connection {
        sender: DialScreen.currentDialer
        signal: "stateChanged()"
        script: { 
            if (DialScreen.currentDialer.state == 1) {
                Status.text += "\nRinging";
            } 
            else if (DialScreen.currentDialer.state == 2) {
                Status.text += "\nConnected";
            } 
            else if (DialScreen.currentDialer.state == 0) {
                Status.text += "\nConnection terminated";
                DialScreen.activeCall = false;
                ClearStatusTimer.running = true;
            } 
            else if (DialScreen.currentDialer.state == 3) {
                Status.text += "\nPhone already engaged";
            }
        }
    }
    //! [1]
}
