/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import QtMobility.gallery 1.1
import "script/mediaart.js" as Script

Rectangle {
    property alias songId: song.item

    color: "black"

    GalleryItem {
        id: song
        gallery: documentGallery

        properties: [
            "title",
            "artist",
            "albumTitle",
            "albumArtist",
            "genre",
            "duration",
            "rating",
            "trackNumber"
        ]
    }

    Flickable {
        anchors.fill: parent

        Item {
            anchors.left: parent.left
            anchors.right: parent.right

            height: ratingEdit.x + ratingEdit.height + 5

            Text {
                id: trackEdit

                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.verticalCenter: titleEdit.verticalCenter
                width: 128

                color: "grey"
                font.pointSize: 20
                horizontalAlignment: Text.AlignRight

                text: song.metaData.trackNumber
            }

            LineEdit {
                id: titleEdit

                anchors.left: trackEdit.right
                anchors.leftMargin: 2
                anchors.top: parent.top
                anchors.topMargin: 2
                anchors.right: closeButton.left
                anchors.rightMargin: 2

                font.pointSize: 20

                text: song.metaData.title
            }

            Text {
                id: closeButton

                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5

                color: "grey"
                font.pointSize: 20

                text: qsTr("Close")

                MouseArea {
                    anchors.fill: parent
                    onClicked: songProperties.close()
                }
            }

            Item {
                id: albumInfo

                anchors.left: parent.left
                anchors.leftMargin: 2
                anchors.top: titleEdit.bottom
                anchors.topMargin: 2
                anchors.right: parent.right
                anchors.rightMargin: 2
                height: albumCover.height + 4

                Image {
                    id: albumCover

                    anchors.left: parent.left
                    anchors.top: parent.top

                    width: 128
                    height: 128

                    fillMode: Image.PreserveAspectFit
                    asynchronous: true
                    source: Script.getAlbumArtUrl(song.metaData.albumArtist, song.metaData.albumTitle)
                }

                Image {
                    anchors.fill: albumCover
                    source: albumCover.status == Image.Error ? "images/nocover.png" : ""
                }

                LineEdit {
                    id: albumTitleEdit

                    anchors.left: albumCover.right
                    anchors.leftMargin: 2
                    anchors.top: parent.top
                    anchors.right: parent.right

                    font.pointSize: 15

                    text: song.metaData.albumTitle
                }

                LineEdit {
                    id: albumArtistEdit

                    anchors.left: albumCover.right
                    anchors.leftMargin: 2
                    anchors.top: albumTitleEdit.bottom
                    anchors.right: parent.right

                    font.pointSize: 12

                    text: song.metaData.albumArtist
                }
            }

            Text {
                id: artistLabel

                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.top: albumInfo.bottom
                anchors.topMargin: 5
                width: 128

                color: "grey"
                font.pointSize: 12

                text: qsTr("Artist")
            }

            LineEdit {
                id: artistEdit

                anchors.left: artistLabel.right
                anchors.leftMargin: 2
                anchors.top: albumInfo.bottom
                anchors.topMargin: 2
                anchors.right: parent.right
                anchors.rightMargin: 2

                font.pointSize: 12

                text: song.metaData.artist
            }

            Text {
                id: genreLabel

                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.top: artistLabel.bottom
                anchors.topMargin: 5
                width: 128

                color: "grey"
                font.pointSize: 12

                text: qsTr("Genre")
            }

            LineEdit {
                id: genreEdit

                anchors.left: genreLabel.right
                anchors.leftMargin: 2
                anchors.top: artistEdit.bottom
                anchors.topMargin: 2
                anchors.right: parent.right
                anchors.rightMargin: 2

                font.pointSize: 12

                text: song.metaData.genre
            }


            Text {
                id: durationLabel

                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.top: genreLabel.bottom
                anchors.topMargin: 5
                width: 128

                color: "grey"
                font.pointSize: 12

                text: qsTr("Duration")
            }

            Text {
                id: durationEdit

                anchors.left: durationLabel.right
                anchors.leftMargin: 5
                anchors.top: genreEdit.bottom
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5

                color: "white"
                font.pointSize: 12

                text: Script.formatDuration(song.metaData.duration)
            }


            Text {
                id: ratingLabel

                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.top: durationLabel.bottom
                anchors.topMargin: 5
                width: 128

                color: "grey"
                font.pointSize: 12

                text: qsTr("Rating")
            }

            Text {
                id: ratingEdit

                anchors.left: ratingLabel.right
                anchors.leftMargin: 5
                anchors.top: durationEdit.bottom
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5

                color: "white"
                font.pointSize: 12

                text: song.metaData.rating
            }
        }
    }
}
