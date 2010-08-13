import Qt 4.7
import QtMobility.gallery 1.1

ListView {
    anchors.fill: parent
    clip: true

    model: GalleryQueryModel {
        gallery: DocumentGallery {}
        rootType: "Album"
        properties: [ "artist", "title" ]
        sortProperties: [ "artist", "title" ]
    }
    delegate: AlbumDelegate {}

    section.property: "artist"
    section.delegate: Item {
        width: parent.width
        height: artistLabel.height
        Text {
            id: artistLabel
            text: section
            color: "white"
            font.pointSize: 15
            anchors.left: parent.left
            anchors.top: parent.top
        }

        Rectangle {
            height: 1
            color: "grey"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: artistLabel.right
            anchors.right: parent.right
        }
    }
}
