/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
    Coordinate { id: topLeftCoordinate; latitude: -28.330; longitude: 154.398}
    Coordinate { id: bottomRightCoordinate; latitude: -28.380; longitude: 154.445}
    Map {
        id: map
        plugin : Plugin { name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.leech"}}
        anchors.fill: parent; size.width: parent.width; size.height: parent.height; zoomLevel: 12
        center: defaultMapCenter

        MapMouseArea {
            onClicked: {
                console.log('clicked map')
                bottomRightCircle.visible = false
                topLeftCircle.visible = false
                centerCircle.visible = false
            }
        }
        MapCircle {
            id: topLeftCircle
            center: topLeftCoordinate
            radius: 500
            visible: false
            MapMouseArea {
                onDoubleClicked: {
                    console.log('doubleclicked topLeft')
                    parent.visible = false
                    bottomRightCircle.visible = true
                }
            }
        }
        MapCircle {
            id: centerCircle
            center: defaultMapCenter
            radius: 500
            visible: true
            MapMouseArea {
                onClicked: {
                    console.log('clicked center')
                    parent.visible = false
                    topLeftCircle.visible = true
                }
            }
        }
        MapCircle {
            id: bottomRightCircle
            center: bottomRightCoordinate
            radius: 500
            visible: false
            MapMouseArea {
                enabled: false
                onClicked: console.log('clicked bottomRight')
                onDoubleClicked: console.log('doubleclicked bottomRight')
            }
        }
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
