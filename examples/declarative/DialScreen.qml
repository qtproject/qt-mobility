import Qt 4.6

//Layout of the DialScreen control
//------------------------------------
//|DialScreen                        |  
//| ---------------------  ____  NumberPad
//| |DialNumber         | /          |
//| ---------------------/          _|__  HangUpButton
//| --------------------/- ------- / |
//| |      |      |      | |     |/  |      
//| |   1  |   2  |   3  | |     |   |    
//| |      |      |      | |     |   |    
//| ---------------------- |     |   |
//| |      |      |      | |     |   |    
//| |   4  |   5  |   6  | |     |   |    
//| |      |      |      | -------   |         
//| ----------------------          _|__  CallButton
//| |      |      |      | ------- / |          
//| |   7  |   8  |   9  | |     |/  |
//| |      |      |      | |     |   |         
//| ---------------------- |     |   |
//| |      |      |      | |     |   |         
//| |   *  |   0  |   #  | |     |   |         
//| |      |      |      | |     |   |         
//| ---------------------- -------   |
//|                                  |
//------------------------------------

//! [0]
Item {
    width: childrenRect.width
    height: childrenRect.height
    property string dialString
    signal dial(string numberToDial)
    signal hangup
    //! [0]

    Rectangle {
        id: DialNumber
        height: 20
        width: NumberPad.width
        anchors.top: parent.top
        anchors.left: parent.left
        color: "white"
        radius: 5
        border.width: 3
        border.color: "black"
        
        Text { 
            text: dialString
            anchors.centerIn: parent
        }
    }

    Grid {
        id: NumberPad
        width: childrenRect.width
        height: childrenRect.height
        anchors.top: DialNumber.bottom
        anchors.left: parent.left
        anchors.topMargin: 5
        columns: 3
        spacing: 5
        
        DialButton { buttonText: "1"; onClicked: { dialString += "1";} }
        DialButton { buttonText: "2"; onClicked: { dialString += "2";} }
        DialButton { buttonText: "3"; onClicked: { dialString += "3";} }
        DialButton { buttonText: "4"; onClicked: { dialString += "4";} }
        DialButton { buttonText: "5"; onClicked: { dialString += "5";} }
        DialButton { buttonText: "6"; onClicked: { dialString += "6";} }
        DialButton { buttonText: "7"; onClicked: { dialString += "7";} }
        DialButton { buttonText: "8"; onClicked: { dialString += "8";} }
        DialButton { buttonText: "9"; onClicked: { dialString += "9";} }
        DialButton { buttonText: "*"; onClicked: { dialString += "*";} }
        DialButton { buttonText: "0"; onClicked: { dialString += "0";} }
        DialButton { buttonText: "#"; onClicked: { dialString += "#";} }
    }
    
    //! [1]
    DialButton {
        id: HangUpButton
        height: { (NumberPad.height / 2) - 2 }
        width: 50
        anchors.top: NumberPad.top
        anchors.left: NumberPad.right
        anchors.leftMargin: 5
        hoverColor: "red"
        color: "crimson"
        onClicked: {
            dialString = ""
            hangup()
        }
        //! [1]       
        Image {
            anchors.centerIn: parent
            source: "hangup.png"
            transformOrigin: "Center"
        }
    }
    
    //! [2]
    DialButton {
        id: CallButton
        width: 50
        height: {(NumberPad.height/2) -2}
        anchors.top: HangUpButton.bottom
        anchors.left: NumberPad.right
        anchors.leftMargin: 5
        anchors.topMargin: 4
        color: "mediumseagreen"
        hoverColor: "lightgreen"
        onClicked: {
            if (dialString != "") {
                dial(dialString)
                dialString = ""
            }
        }
        //! [2]
        
        Image {
            anchors.centerIn: parent
            source: "call.png"
            transformOrigin: "Center"
        }
    }
}
