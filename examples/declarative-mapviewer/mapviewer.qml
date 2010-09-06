/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
import QtMobility.location 1.1

Item {
    width: 500
    height: 500

    focus: true

    Map {
        id: map
        size.width: parent.width
        size.height: parent.height
        zoomLevel: 10
        center: Coordinate {
                    latitude: -27
                    longitude: 153
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

    Keys.onPressed: {
        if (event.key == Qt.Key_Plus) {
            if (map.zoomLevel < map.maximumZoomLevel)
                map.zoomLevel += 1
        } else if (event.key == Qt.Key_Minus) {
            if (map.zoomLevel > map.minimumZoomLevel)
                map.zoomLevel -= 1
        } else if (event.key == Qt.Key_T) {
            if (map.mapType == Map.StreetMap) {
                map.mapType = Map.SatelliteMapDay
            } else if (map.mapType == Map.SatelliteMapDay) {
                map.mapType = Map.StreetMap
            }
        }
    }
}
