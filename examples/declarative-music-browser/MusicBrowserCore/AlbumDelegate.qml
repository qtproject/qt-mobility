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

Package {
    property string state
    property int viewWidth: 0
    property int viewHeight: 0

    signal clicked

    id: albumDelegate

    Item {
        id: coversItem
        Package.name: 'covers'
        width: 128
        height: 128
        transform: Rotation { axis { x: 0; y: 1; z: 0} origin { x: 64; y: 64 } angle: -30 }
    }
    Item {
        id: gridItem;
        Package.name: 'grid';
        width: 192;
        height: 192
    }
    Item {
        Package.name: 'list'
        width: viewWidth
        height: listLoader.height > 192 ? listLoader.height : 192

        Item {
            id: listItem
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 192
        }

        Loader {
            id: listLoader
            anchors.left: listItem.right
            anchors.right: parent.right
            anchors.top: parent.top
        }
    }

    Item {
        Package.name: 'fullScreen'
        width: viewWidth
        height: viewHeight

        Item {
            id: fullScreenItem
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 192
        }

        Loader {
            id: fullScreenLoader
            anchors.left: fullScreenItem.right
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }
    }

    Item {
        id: albumInfo
        anchors.fill: parent

        Image {
            id: albumImage

            property url albumUrl: Utility.getAlbumArtThumbnailUrl(artist, title)

            width: 128
            height: 128
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            source: albumUrl != "" ? albumUrl : "images/nocover.png"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
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

        Text {
            id: artistLabel
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: titleLabel.bottom
            text: artist
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: "white"
        }

        MouseArea {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: artistLabel.bottom
            onClicked: albumDelegate.clicked()
        }

        states: [
            State {
                when: albumDelegate.state == 'cover'
                ParentChange { target: albumInfo; parent: coversItem }
                PropertyChanges { target: titleLabel; visible: false }
                PropertyChanges { target: artistLabel; visible: false }
            },
            State {
                when: albumDelegate.state == 'grid'
                ParentChange { target: albumInfo; parent: gridItem }
            },
            State {
                when: albumDelegate.state == 'list'
                ParentChange { target: albumInfo; parent: listItem }
                PropertyChanges { target: listLoader; sourceComponent: listView }
            },
            State {
                when: albumDelegate.state == 'fullScreen' && fullScreenItem.parent.ListView.isCurrentItem
                PropertyChanges { target: fullScreenLoader; sourceComponent: fullScreenView }
                ParentChange { target: albumInfo; parent: fullScreenItem }
            },
            State {
                when: albumDelegate.state == 'fullScreen'
                PropertyChanges { target: albumInfo; visible: false; }
            }
        ]
    }

    Component {
        id: listView

        Column {
            id: songColumn

            Repeater {
                model: DocumentGalleryModel {
                    rootType: DocumentGallery.Audio
                    rootItem: itemId
                    properties: [ "trackNumber", "title", "duration" ]
                    sortProperties: [ "trackNumber" ]
                }
                delegate: SongDelegate { width: songColumn.width }
            }
        }
    }

    Component {
        id: fullScreenView

        ListView {
            model: DocumentGalleryModel {
                rootType: DocumentGallery.Audio
                rootItem: itemId
                properties: [ "trackNumber", "title", "duration" ]
                sortProperties: [ "trackNumber" ]
            }
            delegate: SongDelegate { width: parent.width }
        }
    }
}
