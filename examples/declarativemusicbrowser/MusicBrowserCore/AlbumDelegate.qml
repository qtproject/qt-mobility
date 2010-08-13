import Qt 4.7
import QtMobility.gallery 1.1
import "script/mediaart.js" as Script

Item {
    width: parent.width
    height: songView.height > 192 ? songView.height : 192

    Item {
        id: albumInfo
        width: 192
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

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
    }

    ListView {
        id: songView
        anchors.left: albumInfo.right
        anchors.right: parent.right
        anchors.top: parent.top
        height: count * 32

        model: GalleryQueryModel {
            gallery: DocumentGallery {}
            rootType: "Audio"
            rootItem: itemId
            properties: [ "trackNumber", "title", "duration" ]
            sortProperties: [ "trackNumber" ]
        }
        delegate: Item {
            width: parent.width
            height: 32

            Text {
                id: trackLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                width: 48
                text: trackNumber
                color: "white"
            }

            Text {
                id: titleLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: trackLabel.right
                text: title
                color: "white"
            }

            Text {
                id: durationLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                text: Script.formatDuration(duration)
                color: "white"
            }
        }
    }
}
