import Qt 4.7

Rectangle {
    property string text: ""
    property string defaultText: ""
    property bool cancelable: true
    property int size: 0
    signal confirmed(string input);
    
    id: page
    opacity: 0

    width: dialogText.width + 30 + size; height: dialogText.height + okButton.height + inputText.height + 44
    anchors.verticalCenter: mainWindow.verticalCenter
    anchors.horizontalCenter: mainWindow.horizontalCenter

    border.width: 1; color: "lightgray"; radius: 5

    Text {
        id: dialogText
        text: page.text
        x: 15; y: 15
        color: activePalette.buttonText
    }

    Rectangle {
        id: inputArea
        width: page.width - 30 
        height: inputText.height + 4
        border.width: 1; color: "white"; radius: 1
        anchors.left: dialogText.left
        anchors.top: dialogText.bottom; anchors.topMargin: 7
    }

    TextEdit {
        id: inputText
        text: page.defaultText
        width: inputArea.width - 10
        anchors.verticalCenter: inputArea.verticalCenter
        anchors.horizontalCenter: inputArea.horizontalCenter
    }

    Button {
        id: okButton
        text: "Ok"
        width: 75; height: 25
        anchors.top: inputArea.bottom; anchors.topMargin: 10

        onClicked: {
            page.confirmed(inputText.text);
            forceClose(); 
        }
    }

    Button {
        id: noButton
        text: "Cancel"
        width: 75; height: 25
        anchors.left: page.horizontalCenter; anchors.leftMargin: 5
        anchors.top: inputArea.bottom; anchors.topMargin: 10
    
        onClicked: {
            forceClose();
        }
    }

    function forceClose()
    {
        page.opacity = 0;
        inputText.text = "";
    }

    Component.onCompleted: {
        if (cancelable == false) {
            noButton.opacity = 0;
            okButton.anchors.horizontalCenter = page.horizontalCenter;
        } else {
            okButton.anchors.right = page.horizontalCenter;
            okButton.anchors.rightMargin = 5;
        }
        
    }
}
