/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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
import "MusicBrowserCore"

Rectangle {
    id: browser
    width: 800
    height: 480
    color: "black"

    Flipable {
        property real angle: 0

        id: menu
        anchors.fill: parent

        front: Item {
            anchors.fill: parent

            Row {
                anchors.centerIn: parent

                TypeDelegate {
                    icon: "MusicBrowserCore/images/albums.png"
                    itemType: DocumentGallery.Album
                    text: qsTr("Albums")
                    onClicked: {
                        viewLoader.sourceComponent = albumView
                        viewButton.text = text
                        menu.state = "showView"
                    }
                }

                TypeDelegate {
                    icon: "MusicBrowserCore/images/artists.png"
                    itemType: DocumentGallery.Artist
                    text: qsTr("Artists")
                    onClicked: {
                        viewLoader.sourceComponent = artistView
                        viewButton.text = text
                        menu.state = "showView"
                    }
                }

                TypeDelegate {
                    icon: "MusicBrowserCore/images/genres.png"
                    itemType: DocumentGallery.AudioGenre
                    text: qsTr("Genres")
                    onClicked: {
                        viewLoader.sourceComponent = genreView;
                        viewButton.text = text
                        menu.state = "showView"
                    }
                }

                TypeDelegate {
                    icon: "MusicBrowserCore/images/songs.png"
                    itemType: DocumentGallery.Audio
                    text: qsTr("Songs")
                    onClicked: {
                        viewLoader.sourceComponent = songView;
                        viewButton.text = text
                        menu.state = "showView"
                    }
                }
            }

            Text {
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5

                color: "grey"
                font.pointSize: 20

                text: qsTr("Close")

                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.quit()
                }
            }
        }
        back: Item {
            anchors.fill: parent

            Text {
                id: menuButton
                color: "grey"
                font.pointSize: 20
                anchors.top: parent.top
                anchors.left: parent.left
                text: qsTr("Menu")

                MouseArea {
                    anchors.fill: parent
                    onClicked: menu.state = ""
                }
            }

            MenuButton {
                id: viewButton
                anchors.left: menuButton.right
                anchors.top: parent.top
                enabled: viewLoader.status == Loader.Ready && viewLoader.item.backEnabled

                onClicked: viewLoader.item.back();
            }

            MenuButton {
                id: viewBackButton
                anchors.left: viewButton.right
                anchors.top: parent.top
                visible: viewLoader.status == Loader.Ready && viewLoader.item.backEnabled
                text: viewLoader.status == Loader.Ready ? viewLoader.item.subTitle : ""
                enabled: false
            }

            Text {
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5

                color: "grey"
                font.pointSize: 20

                text: qsTr("Back")

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (viewButton.enabled)
                            viewLoader.item.back()
                        else
                            menu.state = ""
                    }
                }
            }

            Loader {
                id: viewLoader
                anchors.top: menuButton.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }

        Component { id: albumView; AlbumView {} }
        Component { id: artistView; CategoryView { categoryType: DocumentGallery.Artist } }
        Component { id: genreView; CategoryView { categoryType: DocumentGallery.AudioGenre } }
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

    SongProperties {
        id: songProperties

        width: parent.width
        height: parent.height
        x: 0
        y: parent.height

        function show(itemId) { songId = itemId; state = 'visible' }
        function close() { songId = undefined; state = "" }

        states: [
            State {
                name: "visible"
                PropertyChanges { target: songProperties; y: 0 }
            }
        ]

        transitions: [
            Transition {
                from: ""
                to: "visible"
                PropertyAnimation { target: songProperties; property: "y"; duration: 300 }
            },
            Transition {
                from: "visible"
                to: ""
                PropertyAnimation { target: songProperties; property: "y"; duration: 200 }
            }
        ]
    }
}
