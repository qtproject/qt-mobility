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
import QtMobility.messaging 1.1

Rectangle {
    width: 320
    height: 480
    ListView {
        id: list
        anchors.fill: parent
        model: MessageModel {
            sortBy: MessageModel.Timestamp
            sortOrder: MessageModel.DescendingOrder
        }
        delegate: Item {
            id: wrapper
            height: 50; width: list.width
            Rectangle {
                id: heading
                height: 50; width: list.width
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        wrapper.state = wrapper.state == 'Details' ? '' : 'Details';
                    }
                }
                Text { id: subjText; text: subject; font.pixelSize: 18; x: 3 }
                Text {
                    text: sender; color: "gray"; font.pixelSize: 10
                    anchors.left: subjText.left; anchors.right: dateText.left; anchors.rightMargin: 4
                    elide: Text.ElideRight; anchors.top: subjText.bottom; anchors.topMargin: 6
                }
                Text { id: dateText; text: date; color: "gray"; font.pixelSize: 10
                    anchors.right: parent.right; anchors.top: subjText.bottom
                    anchors.topMargin: 6; anchors.rightMargin: 3
                    MouseArea {
                        anchors.fill: parent
                        onClicked: list.model.showMessage(index)
                    }
                }
                Rectangle { id: separator; y: wrapper.height-1; width: parent.width; height: 1; color: "lightgray" }
                color: ready ? "white" : "yellow"
            }
            Flickable {
                id: bodyView; anchors.top: heading.bottom; anchors.bottom: wrapper.bottom; width: list.width
                contentHeight: bodyText.height; clip: true
                Text { id: bodyText; width: list.width; wrapMode: Text.WordWrap; opacity: 0 }
            }
            states: State {
                name: "Details"
                PropertyChanges { target: wrapper; height: list.height }
                PropertyChanges { target: wrapper.ListView.view; contentY: wrapper.y; interactive: false }
                PropertyChanges { target: bodyText; opacity: 1; text: { body == undefined ? "Loading" : body } }
                PropertyChanges { target: heading; color: "lightsteelblue" }
            }
            transitions: Transition {
                SequentialAnimation {
                    ParallelAnimation {
                        ColorAnimation { property: "color"; duration: 500 }
                        NumberAnimation { duration: 300; properties: "contentY,height" }
                    }
                    PropertyAction { property: "text" }
                    NumberAnimation { properties: "opacity" }
                }
            }
        }
    }
}
