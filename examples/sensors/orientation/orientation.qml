/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.6

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
        transformOrigin: Item.Center
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
            NumberAnimation { matchProperties: "rotation"; easing: "InOutQuad"; duration: 400 }
        }
    }


}
