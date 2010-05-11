import Qt 4.7

Rectangle {
    width: 300; height: 400; color: "white"

    ListModel {
        id: appModel
        ListElement { name: "Time Line View"; icon: "pics/timeline.png" }
        ListElement { name: "Month View"; icon: "pics/month.png" }
        ListElement { name: "Edit"; icon: "pics/edit.png" }
        ListElement { name: "Settings"; icon: "pics/settings.png" }
    }

    Component {
        id: appDelegate
        Item {
            width: 100; height: 100
            Image { id: myIcon; y: 20; anchors.horizontalCenter: parent.horizontalCenter; source: icon; width:80; height:80; smooth:true }
            Text { anchors.top: myIcon.bottom; anchors.horizontalCenter: parent.horizontalCenter; text: name }
        }
    }

    Component {
        id: appHighlight
        Rectangle { width: 80; height: 80; color: "lightsteelblue" }
    }

    GridView {
        anchors.fill: parent
        cellWidth: 100; cellHeight: 100
        model: appModel; delegate: appDelegate
        highlight: appHighlight
        focus: true
    }
}
