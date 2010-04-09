import Qt 4.7

Rectangle {
    property string text: ""
    property bool cancelable: true
    signal confirmed();

    id: page
    opacity: 0
    border.width: 1; color: "lightgray"; radius: 5

    width: 200; height: dialogText.height + okButton.height + 35
    anchors.verticalCenter: mainWindow.verticalCenter
    anchors.horizontalCenter: mainWindow.horizontalCenter

    Text {
        id: dialogText
        text: page.text
        wrapMode: Text.WordWrap
        x: 15; y: 15
        color: activePalette.buttonText
        anchors.horizontalCenter: page.horizontalCenter
    }

    Button {
        id: okButton
        text: "Ok"
        width: 75; height: 25
        anchors.top: dialogText.bottom; anchors.topMargin: 7

        onClicked: {
            page.confirmed();
            page.opacity = 0;
        }
    }

    Button {
        id: noButton
        text: "Cancel"
        width: 75; height: 25
        anchors.left: page.horizontalCenter; anchors.leftMargin: 5
        anchors.top: dialogText.bottom; anchors.topMargin: 7
    
        onClicked: {
            page.opacity = 0;
        }
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
