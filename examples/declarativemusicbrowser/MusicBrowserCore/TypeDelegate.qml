import Qt 4.7
import QtMobility.gallery 1.1

Item {
    property alias itemType: typeInfo.itemType
    property alias icon: typeIcon.source
    property alias text: typeLabel.text
    signal clicked

    width: 192
    height: 192

    GalleryType {
        id: typeInfo
        gallery: DocumentGallery {}
        properties: [ "count" ]
        live: true
    }

    Image {
        id: typeIcon
        width: 128
        height: 128
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
    }

    Text {
        id: countLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: typeIcon.bottom
        text: typeInfo.metaData.count
        font.pointSize: 30
        color: "gray"
    }

    Text {
        id: typeLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        font.pointSize: 20
        color: "white"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: parent.clicked()
    }
}
