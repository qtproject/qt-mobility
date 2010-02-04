import Qt 4.6

Video {
    id: video
    width: 800
    height: 600
    anchors.fill: parent
    fillMode: Video.PreserveAspectFit
    source: videoUrl

    Text {
        text: video.title
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
    }

    Item {
        width: 800
        height: 120
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom

        Image {
            source: "qrc:/images/play.png"
            x: 375
            y: 25

            visible: !video.playing || video.paused

            MouseRegion {
                anchors.fill: parent
                onClicked: video.play()
            }
        }

        Image {
            source: "qrc:/images/pause.png"
            x: 375
            y: 25

            visible: video.playing && !video.paused

            MouseRegion {
                anchors.fill: parent
                onClicked: video.pause()
            }
        }

        Image {
            source: "qrc:/images/stop.png"
            x: 305
            y: 35

            visible: video.playing

            MouseRegion {
                anchors.fill: parent
                onClicked: video.stop()
            }
        }

        Image {
            source: "qrc:/images/mute.png"
            x: 465
            y: 40

            MouseRegion {
                anchors.fill: parent
                onClicked: {
                    video.muted = !video.muted;
                }
            }

            Image {
                source: "qrc:/images/muted.png"
                anchors.fill: parent

                visible: video.muted
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

                    onClicked: video.volume = (mouse.x - 15) / 200
                }
            }

            Image {
                source: "qrc:/images/volume-grip.png"
                x: video.volume * 200
                y: 0

                MouseRegion {
                    anchors.fill: parent
                    drag.target: parent; drag.axis: "XAxis"; drag.minimumX: 0; drag.maximumX: 200

                    onPositionChanged: video.volume = parent.x / 200
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

                    onClicked: video.position = (mouse.x - 15) * video.duration / 760
                    enabled: video.duration > 0
                }
            }

            Image {
                source: "qrc:/images/progress-grip.png"
                x: 760 * video.position / video.duration
                y: 0
                visible: video.duration > 0

                MouseRegion {
                    anchors.fill: parent
                    drag.target: parent; drag.axis: "XAxis"; drag.minimumX: 0; drag.maximumX: 760

                    onReleased: video.position = parent.x / 760
                }
            }
        }
    }
}
