import Qt 4.7
import QtMobility.messaging 1.1

Rectangle {
    width: 320
    height: 480
    ListView {
        id: list
        anchors.fill: parent
        model: MessageModel {
            sortBy: MessageModel.Timestamp
            sortOrder: MessageModel.DescendingOrder
        }
        delegate: Item {
            id: wrapper
            height: 50; width: list.width
            Rectangle {
                id: heading
                height: 50; width: list.width
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        wrapper.state = wrapper.state == 'Details' ? '' : 'Details';
                    }
                }
                Text { id: subjText; text: subject; font.pixelSize: 18; x: 3 }
                Text {
                    text: sender; color: "gray"; font.pixelSize: 10
                    anchors.left: subjText.left; anchors.right: dateText.left; anchors.rightMargin: 4
                    elide: Text.ElideRight; anchors.top: subjText.bottom; anchors.topMargin: 6
                }
                Text { id: dateText; text: date; color: "gray"; font.pixelSize: 10
                    anchors.right: parent.right; anchors.top: subjText.bottom
                    anchors.topMargin: 6; anchors.rightMargin: 3
                    MouseArea {
                        anchors.fill: parent
                        onClicked: list.model.showMessage(index)
                    }
                }
                Rectangle { id: separator; y: wrapper.height-1; width: parent.width; height: 1; color: "lightgray" }
                color: ready ? "white" : "yellow"
            }
            Flickable {
                id: bodyView; anchors.top: heading.bottom; anchors.bottom: wrapper.bottom; width: list.width
                contentHeight: bodyText.height; clip: true
                Text { id: bodyText; width: list.width; wrapMode: Text.WordWrap; opacity: 0 }
            }
            states: State {
                name: "Details"
                PropertyChanges { target: wrapper; height: list.height }
                PropertyChanges { target: wrapper.ListView.view; contentY: wrapper.y; interactive: false }
                PropertyChanges { target: bodyText; opacity: 1; text: { body == undefined ? "Loading" : body } }
                PropertyChanges { target: heading; color: "lightsteelblue" }
            }
            transitions: Transition {
                SequentialAnimation {
                    ParallelAnimation {
                        ColorAnimation { property: "color"; duration: 500 }
                        NumberAnimation { duration: 300; properties: "contentY,height" }
                    }
                    PropertyAction { property: "text" }
                    NumberAnimation { properties: "opacity" }
                }
            }
        }
    }
}
