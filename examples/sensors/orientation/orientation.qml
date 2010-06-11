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
import QtMobility.sensors 1.0

Rectangle {
    width: 800
    height: 422
    color: "black"

    OrientationSensor {
        id: orientation

        onReadingChanged: {
            if (reading.orientation == OrientationReading.TopUp)
                content.state = "TopUp";
            else if (reading.orientation == OrientationReading.TopDown)
                content.state = "TopDown";
            else if (reading.orientation == OrientationReading.LeftUp)
                content.state = "LeftUp";
            else if (reading.orientation == OrientationReading.RightUp)
                content.state = "RightUp";
            else if (reading.orientation == OrientationReading.FaceUp)
                content.state = "FaceUp";
            else if (reading.orientation == OrientationReading.FaceDown)
                content.state = "FaceDown";
            else
                content.state = "";
        }
    }
    Component.onCompleted: orientation.start()

    Item {
        id: content
        anchors.centerIn: parent
        Text {
            id: text
            text: "Orientation: Unknown"
            color: "white"
            font.pointSize: 24
            anchors.centerIn: parent
        }

        states: [
            State {
                name: "TopUp"
                changes: [
                    PropertyChanges { target: text; text: "Orientation: TopUp" }
                ]
            },
            State {
                name: "TopDown"
                changes: [
                    PropertyChanges { target: content; rotation: 180 },
                    PropertyChanges { target: text; text: "Orientation: TopDown" }
                ]
            },
            State {
                name: "LeftUp"
                changes: [
                    PropertyChanges { target: content; rotation: -90 },
                    PropertyChanges { target: text; text: "Orientation: LeftUp" }
                ]
            },
            State {
                name: "RightUp"
                changes: [
                    PropertyChanges { target: content; rotation: 90 },
                    PropertyChanges { target: text; text: "Orientation: RightUp" }
                ]
            },
            State {
                name: "FaceUp"
                changes: [
                    PropertyChanges { target: text; text: "Orientation: FaceUp" }
                ]
            },
            State {
                name: "FaceDown"
                changes: [
                    PropertyChanges { target: text; text: "Orientation: FaceDown" }
                ]
            }
        ]

        transitions: Transition {
            NumberAnimation { properties: "rotation"; easing.type: Easing.OutBounce; duration: 400 }
        }
    }
}

