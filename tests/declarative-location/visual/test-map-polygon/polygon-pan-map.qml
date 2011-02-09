/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

import Qt 4.7
import QtMobility.location 1.2
import "../shared-qml" as Shared
import QtQuick 1.0

Rectangle {
    id: page; width: 200; height: 300; focus: true
    Coordinate { id: defaultMapCenter; latitude: -28.36; longitude: 154.42}

    Map {
        id: map
        plugin : Plugin { name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.leech"}}
        anchors.fill: parent; size.width: parent.width; size.height: parent.height; zoomLevel: 12
        center: defaultMapCenter
        MapPolygon {
            id: polygon
            color: "black"
            Coordinate {
                id: polygonTopLeftCoordinate
                latitude: -28.35
                longitude: 154.4
            }
            Coordinate {
                id: polygonRightCoordinate
                latitude: -28.34
                longitude: 154.45
            }
            Coordinate {
                id: polygonBottomLeftCoordinate
                latitude: -28.33
                longitude: 154.4
            }
        }
    }
    MouseArea {
        anchors.fill: parent
        property bool mouseDown : false
        property int lastX : -1
        property int lastY : -1
        onPressed : {
            mouseDown = true
            lastX = mouse.x
            lastY = mouse.y
        }
        onReleased : {
            mouseDown = false
            lastX = -1
            lastY = -1
        }
        onPositionChanged: {
            if (mouseDown) {
                var dx = mouse.x - lastX
                var dy = mouse.y - lastY
                map.pan(-dx, -dy)
                lastX = mouse.x
                lastY = mouse.y
            }
        }
        onDoubleClicked: {
            map.center = map.toCoordinate(Qt.point(mouse.x, mouse.y))
            if (map.zoomLevel < map.maximumZoomLevel)
                map.zoomLevel += 1
        }
    }
    Coordinate { id: coordinate1; latitude: -28.35; longitude: 154.42}
    Coordinate { id: coordinate2; latitude: -28.35; longitude: 154.43}
    Shared.ToolBar {
        id: toolbar1
        height: 40; width: parent.width
        anchors.bottom: toolbar2.top
        button1Label: ""; button2Label: ""; button3Label: ""
        onButton1Clicked: {polygon.addCoordinate(coordinate1)}
        onButton2Clicked: {polygon.addCoordinate(coordinate2)}
        onButton3Clicked: {polygon.addCoordinate(coordinate1)}
    }
    Shared.ToolBar {
        id: toolbar2
        height: 40; width: parent.width
        anchors.bottom: toolbar3.top
        button1Label: ""; button2Label: ""; button3Label: ""
        onButton1Clicked: {polygon.removeCoordinate(coordinate1)}
        onButton2Clicked: {polygon.removeCoordinate(coordinate2)}
        onButton3Clicked: {polygon.removeCoordinate(coordinate1)}
    }
    Shared.ToolBar {
        id: toolbar3
        height: 40; width: parent.width
        anchors.bottom: parent.bottom
        button1Label: ""; button2Label: ""; button3Label: ""
        onButton1Clicked: {polygon.addCoordinate(coordinate1)}
        onButton2Clicked: {polygon.removeCoordinate(coordinate2)}
        onButton3Clicked: {polygon.removeCoordinate(coordinate1)}
    }
} // page
