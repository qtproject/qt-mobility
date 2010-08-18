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
    property alias state: albumInfo.state
    property int viewWidth: 0
    property int viewHeight: 0

    signal clicked


    id: albumDelegate

    Item { id: stackItem; Package.name: 'stack'; width: 128; height: 128; }
    Item { id: gridItem; Package.name: 'grid'; width: 192; height: 192 }
    Item {

        Package.name: 'verticalList'
        width: viewWidth
        height: verticalSongsLoader.height > 192 ? verticalSongsLoader.height : 192

        Item {
            id: verticalListItem
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 192
        }

        Loader {
            id: verticalSongsLoader
            anchors.left: verticalListItem.right
            anchors.right: parent.right
            anchors.top: parent.top
        }
    }

    Item {
        property string albumTitle: title

        Package.name: 'horizontalList'
        width: viewWidth
        height: viewHeight

        Item {
            id: horizontalListItem
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 192
        }

        Loader {
            id: horizontalSongsLoader
            anchors.left: horizontalListItem.right
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

        MouseArea {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: artistLabel.bottom
            onClicked: albumDelegate.clicked()
        }

        states: [
            State {
                name: 'inStack'
                ParentChange { target: albumInfo; parent: stackItem }
                PropertyChanges { target: songsLoader; sourceComponent: undefined }
            },
            State {
                name: 'inGrid'
                ParentChange { target: albumInfo; parent: gridItem }
                PropertyChanges { target: titleLabel; visible: true }
                PropertyChanges { target: artistLabel; visible: true }
                PropertyChanges { target: verticalSongsLoader; sourceComponent: undefined }
                PropertyChanges { target: horizontalSongsLoader; sourceComponent: undefined }
            },
            State {
                name: 'inVerticalList'
                ParentChange { target: albumInfo; parent: verticalListItem }
                PropertyChanges { target: verticalSongsLoader; sourceComponent: verticalSongView }
                PropertyChanges { target: titleLabel; visible: true }
            },
            State {
                name: 'inHorizontalList'
                ParentChange { target: albumInfo; parent: horizontalListItem }
                PropertyChanges { target: horizontalSongsLoader; sourceComponent: horizontalSongView }
                PropertyChanges { target: titleLabel; visible: true }
            }
        ]
    }

    Component {
        id: verticalSongView

        Column {
            id: songColumn

            Repeater {
                model: GalleryQueryModel {
                    gallery: documentGallery
                    rootType: "Audio"
                    rootItem: itemId
                    properties: [ "trackNumber", "title", "duration" ]
                    sortProperties: [ "trackNumber" ]
                }
                delegate: SongDelegate { width: songColumn.width }
            }
        }
    }

    Component {
        id: horizontalSongView

        ListView {
            model: GalleryQueryModel {
                gallery: documentGallery
                rootType: "Audio"
                rootItem: itemId
                properties: [ "trackNumber", "title", "duration" ]
                sortProperties: [ "trackNumber" ]
            }
            delegate: SongDelegate { width: parent.width }
        }
    }
}
