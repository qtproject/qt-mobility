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
        radius: 500
    }

    LandmarkModel {
        id: landmarkModel
        autoUpdate: true
        onModelChanged: console.log("Model changed, landmark count: " + count)
        filter: proximityFilter
    }

    Map {
        id: map
        size.width: parent.width
        size.height: parent.height
        zoomLevel: 15
        center: myPositionSource.position.coordinate
        onCenterChanged: {
            var distance = center.distanceTo(map.toCoordinate(Qt.point(0,0)))
            console.log("Center changed, radius for proximity filter will be set to: " + distance / 2)
            proximityFilter.radius = distance / 2
        }
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
            console.log("Clicked, setting import file to get landmarks");
            landmarkModel.importFile = "myImportedLandmarks.gpx"
            myPositionSource.start()
        }
        onButton3Clicked: {
            console.log("Clicked, setting map center to follow the source");
            map.center = myPositionSource.position.coordinate
        }
    }
}
