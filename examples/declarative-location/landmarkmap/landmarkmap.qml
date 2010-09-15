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
import "common" as Common
import "mobile" as Mobile

Item {
    id: page
    width: 320
    height: 480
    focus: true

    MouseArea {
        anchors.fill: parent
        onDoubleClicked: {
            map.center = map.toCoordinate(Qt.point(mouse.x, mouse.y))
        }
    }

    PositionSource {
        id: myPositionSource
        active: true
        updateInterval: 5000
        onPositionChanged: console.log("Position changed")
    }

    LandmarkProximityFilter {
        id: proximityFilter
        coordinate: map.center
    }

    LandmarkModel {
        id: landmarkModel
        autoUpdate: true
        onModelChanged: console.log("Model changed, landmark count: " + count)
        filter: proximityFilter
        limit: 10
    }

    Component {
        id: landmarkDelegate
        Item {
            width: 50; height: 20
            x: map.toScreenPosition(landmark.coordinate).x
            y: map.toScreenPosition(landmark.coordinate).y
            Image {
                id: landmarkIcon
                source: "mobile/images/landmarkstar.png"
                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        console.log("Landmark clicked, setting visible: " + landmark.name)
                        landmarkNameText.visible = true
                    }
                }
            }
            Text {
                id: landmarkNameText
                anchors.top: landmarkIcon.bottom
                text:  landmark.name
                visible: false
            }
        }
    }

    Map {
        id: map
        size.width: parent.width
        size.height: parent.height
        zoomLevel: 1
        center: myPositionSource.position.coordinate
        onZoomLevelChanged: {
            var distance = center.distanceTo(map.toCoordinate(Qt.point(0,0)))
            console.log("Zoom changed, radius for proximity filter will be set to: " + distance / 2)
            proximityFilter.radius = distance / 2
        }
        onCenterChanged: console.log("Center changed")
    }

    Mobile.TitleBar { id: titleBar; z: 5; width: parent.width - statusBar.width; height: 40; opacity: 0.9 }
    Mobile.StatusBar { id: statusBar; z: 6; width: 80; height: titleBar.height; opacity: titleBar.opacity; anchors.right: parent.right}

    Repeater {
        id: landmarkView
        model: landmarkModel
        delegate: landmarkDelegate
    }

    Common.Slider {
         id: zoomSlider;
         minimum: 1; maximum: 18;
         anchors {
             bottom: toolbar.top; left: parent.left; right: parent.right;
             bottomMargin: 5; rightMargin: 5; leftMargin: 5
         }
         onValueChanged: {
             console.log("Zoom slider value changed to: " + value)
             map.zoomLevel = value
         }
    }
    Mobile.ToolBar {
        id: toolbar
        height: 40; width: parent.width
        anchors.bottom: parent.bottom
        button1Label: "NMEA"; button2Label: "Import"; button3Label: "Whereami"
        onButton1Clicked: {
            console.log("Clicked, setting nmea log as source");
            myPositionSource.nmeaSource = "nmealog.txt"
            myPositionSource.start()
        }
        onButton2Clicked: {
            console.log("Clicked, setting import file to get landmarks, count was: " + landmarkModel.count);
            landmarkModel.importFile = "AUS-PublicToilet-Queensland.gpx"
            myPositionSource.start()
        }
        onButton3Clicked: {
            console.log("Clicked, setting map center to follow the source");
            map.center = myPositionSource.position.coordinate
        }
    }
}
