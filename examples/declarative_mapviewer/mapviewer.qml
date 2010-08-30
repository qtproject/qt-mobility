/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

        MapCircleObject {
            center: Coordinate {
                        latitude: -27.1
                        longitude: 153
                    }
            radius: 10000.0
            color: "#808080FF"
        }

        MapCircleObject {
            center: Coordinate {
                        latitude: -27.1
                        longitude: 153
                    }
            radius: 8000.0
        }

        MapCircleObject {
            center: Coordinate {
                        latitude: -27.1
                        longitude: 153
                    }
            radius: 6000.0
            color: "#80FF8080"
        }

        MapRectangleObject {
            topLeft:        map.toCoordinate(Qt.point(map.size.width / 3.0, map.size.height * 2.0 / 3.0))        
            bottomRight:    map.toCoordinate(Qt.point(map.size.width * 2.0 / 3.0, map.size.height / 3.0))   
            color: "#8080FF80"
        }

        MapPolygonObject {
            color: "#80808080"
            Coordinate {
                latitude: -27.1
                longitude: 153.1
            }
            Coordinate {
                latitude: -27.0
                longitude: 153.0
            }
            Coordinate {
                latitude: -27.0
                longitude: 153.2
            }
        }

        MapPolylineObject {
            Coordinate {
                latitude: -27.2
                longitude: 153.1
            }
            Coordinate {
                latitude: -27.1
                longitude: 153.0
            }
            Coordinate {
                latitude: -27.1
                longitude: 153.2
            }
        }

        MapTextObject {
            coordinate:     Coordinate {
                                latitude: -27.1
                                longitude: 153.1
                            }
            text: "Testing text"
            font.underline: true
            color: "#800000"
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
