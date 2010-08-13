import Qt 4.7
import QtMobility.gallery 1.1
import "script/mediaart.js" as Script

GridView {
    anchors.fill: parent
    cellWidth: 256
    cellHeight: 192
    clip: true

    model: GalleryQueryModel {
        gallery: DocumentGallery {}
        rootType: "Album"
        properties: [ "title", "artist" ]
        sortProperties: [ "title" ]
    }
    delegate:  Item {
        width: 256
        height: 192

        Image {
            id: albumImage
            width: 128
            height: 128
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            source: Script.getAlbumArtUrl(artist, title)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
        }

        Image {
            anchors.fill: albumImage
            source: albumImage.status == Image.Error ? "images/nocover.png" : ""
        }

        Text {
            id: titleLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: albumImage.bottom
            text: title
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: "white"
        }

        Text {
            id: artistLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: titleLabel.bottom
            text: artist
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: "white"
        }
    }
}
