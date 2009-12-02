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
        x: 0
        y: 470
        width: 800
        height: 120

        Image {
            source: "qrc:/images/play.png"
            x: 375
            y: 25

            visible: video.state != Video.Playing

            MouseRegion {
                anchors.fill: parent
                onClicked: video.state = Video.Playing;
            }
        }

        Image {
            source: "qrc:/images/pause.png"
            x: 375
            y: 25

            visible: video.state == Video.Playing

            MouseRegion {
                anchors.fill: parent
                onClicked: video.state = Video.Paused
            }
        }

        Image {
            source: "qrc:/images/stop.png"
            x: 305
            y: 35

            visible: video.state != Video.Stopped

            MouseRegion {
                anchors.fill: parent
                onClicked: video.state = Video.Stopped
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

                    onClicked: video.position = (mouse.x - 15) / 760
                }
            }

            Image {
                source: "qrc:/images/progress-grip.png"
                x: 760 * video.position
                y: 0

                MouseRegion {
                    anchors.fill: parent
                    drag.target: parent; drag.axis: "XAxis"; drag.minimumX: 0; drag.maximumX: 760

                    onReleased: video.position = parent.x / 760
                }
            }
        }
    }
}

