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

Package {
    property alias state: albumWrapper.state

    Item { id: stackItem; Package.name: 'stack'; width: 128; height: 128; }
    Item { id: listItem; Package.name: 'list'; width: parent.width; height: 192 }
    Item { id: gridItem; Package.name: 'grid'; width: 192; height: 192 }

    Item {
        id: albumWrapper
        anchors.fill: parent

        Item {
            id: albumInfo
            width: parent.width
            anchors.left: parent.left
            anchors.top: parent.top

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
                visible: false
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
                visible: false
            }
        }

        Loader {
            id: songsLoader
            anchors.top: parent.top
            anchors.left: albumInfo.right
            anchors.right: parent.right
            sourceComponent: undefined
        }

        states: [
            State {
                name: 'inStack'
                ParentChange { target: albumWrapper; parent: stackItem }
            },
            State {
                name: 'inGrid'
                ParentChange { target: albumWrapper; parent: gridItem }
                PropertyChanges { target: titleLabel; visible: true }
                PropertyChanges { target: artistLabel; visible: true }
            },
            State {
                name: 'inList'
                PropertyChanges { target: songsLoader; sourceComponent: songView; }
                PropertyChanges { target: albumInfo; width: 192 }
                PropertyChanges { target: listItem; height: songsLoader.height > 192 ? songsLoader.height : 192 }
                PropertyChanges { target: titleLabel; visible: true }
                ParentChange { target: albumWrapper; parent: listItem; x: 0; y: 0 }
            }
        ]
    }

    Component {
        id: songView

        Column {
            id: songColumn

            Repeater {
                model: GalleryQueryModel {
                    gallery: DocumentGallery {}
                    rootType: "Audio"
                    rootItem: itemId
                    properties: [ "trackNumber", "title", "duration" ]
                    sortProperties: [ "trackNumber" ]
                }
                delegate: Item {
                    width: songColumn.width
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
    }
}
