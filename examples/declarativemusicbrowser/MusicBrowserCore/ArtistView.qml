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
        rootType: "Artist"
        properties: [ "artist" ]
    }
    delegate: Item {
        width: 256
        height: 192

        PathView {
            id: albumView
            width: 256
            height: 128
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            model: GalleryQueryModel {
                gallery: DocumentGallery {}
                rootType: "Album"
                rootItem: itemId
                properties: [ "title", "artist" ]
                sortProperties: [ "artist", "title" ]
            }
            delegate: Item {
                width: 128; height: 128

                Image {
                    id: albumImage
                    anchors.fill: parent
                    source: Script.getAlbumArtUrl(artist, title)
                    fillMode: Image.PreserveAspectFit
                    asynchronous: true
                }
                Image {
                    anchors.fill: parent
                    source: albumImage.status == Image.Error ? "images/nocover.png" : ""
                }
            }
            path: Path {
                startX: 64
                startY: 64
                PathLine { x: 192; y: 64 }
            }
        }

        Text {
            id: artistLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: albumView.bottom
            anchors.bottom: parent.bottom
            text: artist
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: "white"
            font.pointSize: 15
        }
    }
}
