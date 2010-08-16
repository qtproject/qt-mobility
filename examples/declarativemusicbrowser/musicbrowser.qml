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

        Component { id: albumView; AlbumView {} }
        Component { id: artistView; ArtistView {} }
        Component { id: genreView; GenreView {} }
        Component { id: songView; SongView {} }

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
