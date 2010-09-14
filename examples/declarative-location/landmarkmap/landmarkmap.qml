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
    property bool inLandmarkView: false

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
    LandmarkBoxFilter {
        id: boxFilter
        topLeft: map.toCoordinate(Qt.point(0,0))
        bottomRight: map.toCoordinate(Qt.point(page.width,-page.height))
    }
    LandmarkModel {
        id: landmarkModel
        autoUpdate: true
        onModelChanged: console.log("Model changed, landmark count: " + count)
        //filter: proximityFilter
        filter: boxFilter
        limit: 50
    }
    ListView {
        id: landmarkListView;
        model: landmarkModel;
        delegate: landmarkListDelegate;
        z: 5
        width: parent.width - 20;
        height: parent.height -20;
        x: -(parent.width * 1.5);
        highlightFollowsCurrentItem: false
        focus: true
    }

    Component {
        id: landmarkListDelegate
        Item {
            width: 200; height: 20
            Text {
                id: nameField; text: landmark.name
            }
        }
    }

    Component {
        id: landmarkMapDelegate
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

    Rectangle {
        id: background
        anchors.fill: parent;
        Image { source: "mobile/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 0.3 }
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: map.center = map.toCoordinate(Qt.point(mouse.x, mouse.y))
        }

        Map {
            id: map
            anchors.top: background.top
            size.width: parent.width
            size.height: parent.height
            zoomLevel: 1
            center: myPositionSource.position.coordinate
            onZoomLevelChanged: {
                console.log("Zoom changed")
                updateFilters();
            }
            onCenterChanged: {
                console.log("Center changed")
                updateFilters();
            }
            function updateFilters () {
                proximityFilter.radius = map.toCoordinate(Qt.point(0,0)) / 2
                boxFilter.topLeft = map.toCoordinate(Qt.point(0,0))
                boxFilter.bottomRight = map.toCoordinate(Qt.point(background.width, background.height))
                //console.log("zero point coordinate lat: " + map.toCoordinate(Qt.point(0,0)).latitude + " lon: " + map.toCoordinate(Qt.point(0,0)).longitude)
                //console.log("bottom point coordinate lat: " + map.toCoordinate(Qt.point(page.width, page.height)).latitude + " lon: " + map.toCoordinate(Qt.point(page.width, page.height)).longitude)
            }
        }
        Mobile.TitleBar { id: titleBar; z: 5; width: parent.width - statusBar.width; height: 40; opacity: 0.7 }
        Mobile.StatusBar { id: statusBar; z: 6; width: 80; height: titleBar.height; opacity: titleBar.opacity; anchors.right: parent.right}

        Repeater {
            id: landmarkView
            model: landmarkModel
            delegate: landmarkMapDelegate
        }

        Common.Slider {
            id: zoomSlider;
            minimum: 1; maximum: 18;
            anchors {
                bottom: toolbar1.top; left: parent.left; right: parent.right;
                bottomMargin: 5; rightMargin: 5; leftMargin: 5
            }
            onValueChanged: {
                console.log("Zoom slider value changed to: " + value)
                map.zoomLevel = value
            }
        }

        Mobile.ToolBar {
            id: toolbar1
            opacity: titleBar.opacity
            height: 40; width: parent.width
            anchors.bottom: toolbar2.top
            z: 6
            button1Label: "landmarks"; button2Label: "categories"; button3Label: "unassigned"
            onButton1Clicked: {
                console.log("Clicked, toggling landmarks mode");
                if (inLandmarkView == true) inLandmarkView = false;
                else inLandmarkView = true;
            }
            onButton2Clicked: {
                console.log("Clicked, toggling categories mode");
            }
            onButton3Clicked: {
                console.log("Clicked, unassigned");
            }
        }

        Mobile.ToolBar {
            id: toolbar2
            opacity: toolbar1.opacity
            height: 40; width: parent.width
            anchors.bottom: parent.bottom
            z: 6
            button1Label: "nmealog.txt"; button2Label: "mydb.gpx"; button3Label: "locate me"
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
        states: [State {
                name: "CategoryView"
                PropertyChanges { target: background; x: -background.width }
            }, State {
                name: "LandmarkView"
                when: inLandmarkView == true
                PropertyChanges { target: map; x: -map.width }
                PropertyChanges { target: zoomSlider; x: -map.width}
                PropertyChanges { target: toolbar1; button1Label: "Back" }
                PropertyChanges { target: landmarkListView; x: 0 }
            }]
        transitions: Transition {
            NumberAnimation { properties: "x"; duration: 500; easing.type: Easing.InOutQuad }
        }
    }
}
