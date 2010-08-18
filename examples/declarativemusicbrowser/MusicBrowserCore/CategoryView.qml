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

Item {
    property string subTitle: ""
    property bool backEnabled: false
    property alias categoryType: categoryModel.rootType

    function back() { state = 'categories'}

    id: categoryView
    clip: true

    GridView {
        id: categoryList
        anchors.fill: parent
        cellWidth: 256
        cellHeight: 192

        model:  GalleryQueryModel {
            id: categoryModel
            gallery: documentGallery
            properties: [ "title" ]
            sortProperties: [ "title" ]
        }
        delegate: Item {
            id: categoryDelegate

            width: 256
            height: 192

            VisualDataModel {
                id: visualModel

                model: GalleryQueryModel {
                    id: galleryModel
                    gallery: documentGallery
                    rootType: "Album"
                    rootItem: itemId
                    properties: [ "artist", "title" ]
                    sortProperties: [ "artist", "title" ]
                }
                delegate: AlbumDelegate {
                    state: categoryDelegate.state
                    viewWidth: songsView.width
                    viewHeight: songsView.height
                }
            }

            Item {
                anchors.fill: parent
                visible: categoryView.state == 'categories'

                PathView {
                    id: albumView
                    anchors.top: parent.top
                    anchors.bottom: categoryLabel.top
                    anchors.left: parent.left
                    anchors.right: parent.right

                    model: visualModel.parts.stack

                    path: Path {
                        startX: 64
                        startY: height / 2
                        PathLine { x: width - 64; y: height / 2 }
                    }
                }

                Text {
                    id: categoryLabel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    text: title
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    color: "white"
                    font.pointSize: 15
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        categoryView.state = 'albums'
                        categoryDelegate.state = 'inVerticalList'
                    }
                }
            }

            ListView {
                id: songsView
                anchors.fill: parent
                model: visualModel.parts.verticalList
                interactive: false
            }

            states: [
                State {
                    name: "inStack"
                    when: categoryView.state == 'categories'
                },
                State {
                    name: "inVerticalList"
                    PropertyChanges { target: categoryView; subTitle: title }
                    PropertyChanges { target: songsView; interactive: true }
                    ParentChange { target: songsView; parent: categoryView }
                }
            ]
        }
    }

    state: 'categories'
    states: [
        State {
            name: 'categories'
            PropertyChanges { target: categoryView; backEnabled: false }
        },
        State {
            name: 'albums'
            PropertyChanges { target: categoryView; backEnabled: true }
        }
    ]
}
