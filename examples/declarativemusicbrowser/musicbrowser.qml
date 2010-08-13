import Qt 4.7
import QtMobility.gallery 1.0
import "MusicBrowserCore"

Rectangle {
    width: 800
    height: 480
    color: "black"

    Flipable {
        property real angle: 0

        id: menu
        anchors.fill: parent

        front: Item {
            width: 800
            height: 480

            Row {
                anchors.centerIn: parent

                TypeDelegate {
                    icon: "MusicBrowserCore/images/albums.png"
                    itemType: "Album"
                    text: qsTr("Albums")
                    onClicked: {
                        viewLoader.sourceComponent = albumView
                        viewLabel.text = text
                        menu.state = "showView"
                    }
                }

                TypeDelegate {
                    icon: "MusicBrowserCore/images/artists.png"
                    itemType: "Artist"
                    text: qsTr("Artists")
                    onClicked: {
                        viewLoader.sourceComponent = artistView
                        viewLabel.text = text
                        menu.state = "showView"
                    }
                }

                TypeDelegate {
                    icon: "MusicBrowserCore/images/genres.png"
                    itemType: "AudioGenre"
                    text: qsTr("Genres")
                    onClicked: {
                        viewLoader.sourceComponent = genreView;
                        viewLabel.text = text
                        menu.state = "showView"
                    }
                }

                TypeDelegate {
                    icon: "MusicBrowserCore/images/songs.png"
                    itemType: "Audio"
                    text: qsTr("Songs")
                    onClicked: {
                        viewLoader.sourceComponent = songView;
                        viewLabel.text = text
                        menu.state = "showView"
                    }
                }
            }
        }
        back: Item {
            width: 800
            height: 480

            Text {
                id: viewLabel
                color: "white"
                font.pointSize: 20
                anchors.top: parent.top
                anchors.left: parent.left

            }

            Text {
                id: backButton
                color: "gray"
                font.pointSize: 20
                anchors.top: parent.top
                anchors.right: parent.right
                text: qsTr("Back")

                MouseArea {
                    anchors.fill: parent
                    onClicked: menu.state = ""
                }
            }

            Loader {
                id: viewLoader
                anchors.top: viewLabel.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }

        Component { id: albumView; AlbumView { anchors.fill: parent } }
        Component { id: artistView; ArtistView { anchors.fill: parent } }
        Component { id: genreView; GenreView { anchors.fill: parent } }
        Component { id: songView; SongView { anchors.fill: parent } }

        transform: Rotation {
            origin.x: menu.width / 2; origin.y: menu.height / 2;
            axis.x: 1; axis.y: 0; axis.z: 0;
            angle: menu.angle;
        }

        states: [
            State {
                name: "showView"
                PropertyChanges {
                    target: menu
                    angle: 180
                }
            }
        ]

        transitions: Transition {
            NumberAnimation { properties: "angle"; duration: 1000 }
        }
    }
}
