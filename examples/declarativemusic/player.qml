import Qt 4.6


Item {
    width: 800
    height: 400

    Audio {
        id: audio
        source: audioUrl
    }

    Image {
        id: coverArt
        source: audio.coverArtUriLarge
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: title.top
    }
    Text {
        id: title
        text: audio.title
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: albumTitle.top
    }
    Text {
        id: albumTitle
        text: audio.albumTitle
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: albumArtist.top
    }
    Text {
        id: albumArtist
        text: audio.albumArtist
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: controls.top
    }
    Item {
        id: controls
        width: 800
        height: 120
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        Image {
            source: "qrc:/images/play.png"
            x: 375
            y: 25

            visible: !audio.playing || audio.paused

            MouseRegion {
                anchors.fill: parent
                onClicked: audio.play()
            }
        }

        Image {
            source: "qrc:/images/pause.png"
            x: 375
            y: 25

            visible: audio.playing && !audio.paused

            MouseRegion {
                anchors.fill: parent
                onClicked: audio.pause()
            }
        }

        Image {
            source: "qrc:/images/stop.png"
            x: 305
            y: 35

            visible: audio.playing

            MouseRegion {
                anchors.fill: parent
                onClicked: audio.stop()
            }
        }

        Image {
            source: "qrc:/images/mute.png"
            x: 465
            y: 40

            MouseRegion {
                anchors.fill: parent
                onClicked: {
                    audio.muted = !audio.muted;
                }
            }

            Image {
                source: "qrc:/images/muted.png"
                anchors.fill: parent

                visible: audio.muted
            }
        }

        Item {
            x: 535
            y: 50
            width: 230
            height: 40

            Image {
                source: "qrc:/images/volume.png"
                x: 0
                y: 5

                MouseRegion {
                    anchors.fill: parent

                    onClicked: audio.volume = (mouse.x - 15) / 200
                }
            }

            Image {
                source: "qrc:/images/volume-grip.png"
                x: audio.volume * 200
                y: 0

                MouseRegion {
                    anchors.fill: parent
                    drag.target: parent; drag.axis: "XAxis"; drag.minimumX: 0; drag.maximumX: 200

                    onPositionChanged: audio.volume = parent.x / 200
                }
            }
        }

        Item {
            x: 5
            y: 0
            width: 790
            height: 30

            Image {
                source: "qrc:/images/progress.png"
                x: 0
                y: 5

                MouseRegion {
                    anchors.fill: parent

                    onClicked: audio.position = (mouse.x - 15) * audio.duration / 760
                    enabled: audio.duration > 0
                }
            }

            Image {
                source: "qrc:/images/progress-grip.png"
                x: 760 * audio.position / audio.duration
                y: 0
                visible: audio.duration > 0

                MouseRegion {
                    anchors.fill: parent
                    drag.target: parent; drag.axis: "XAxis"; drag.minimumX: 0; drag.maximumX: 760

                    onReleased: audio.position = parent.x / 760
                }
            }
        }
    }
}
