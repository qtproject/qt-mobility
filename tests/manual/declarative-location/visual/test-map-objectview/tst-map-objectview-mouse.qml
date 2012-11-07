/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
import QtQuickTest 1.0
import QtMobility.location 1.2
import "../shared-qml" as Shared

Item {
    id: masterItem
    width: 200
    height: 350
    // General-purpose elements for the test:
    Plugin { id: testPlugin; name : "nokia"; PluginParameter {name: "mapping.host"; value: "for.nonexistent"}}
    Coordinate{ id: mapDefaultCenter; latitude: 10; longitude: 30}

    LandmarkModel {
        id: landmarkModel
        autoUpdate: true
        onLandmarksChanged: console.log('landmarks changed count is: ' + landmarks.length)
    }

    Map {
        id: landmarkMap;
        center: mapDefaultCenter
        plugin: testPlugin;
        anchors.fill: parent; size.width: parent.width; size.height: parent.height;
        zoomLevel: 2

        MapMouseArea {
            onClicked: console.log('map mouse area clicked')
        }

        MapCircle {
            color: "black"
            visible: true
            radius: 500000
            center: mapDefaultCenter
            MapMouseArea {
                onClicked: console.log('mouse area of a standalone circle got clicked')
            }
        }

        MapObjectView {
            id: circle_basic_view
            model: landmarkModel
            visible: true
            delegate: Component {
                id: circleMapDelegate
                MapCircle {
                    color: "white"
                    radius: 500000
                    center: landmark.coordinate
                    MapMouseArea {
                        onClicked: {
                            console.log('mouse area of delegate got clicked');
                            (parent.visible == true)? parent.visible = false : parent.visible = true
                        }
                    }
                }
            }
        }
    }
    Shared.ToolBar {
        id: toolbar3
        height: 40; width: parent.width
        anchors.bottom: parent.bottom
        button1Label: ""; button2Label: ""; button3Label: ""
        onButton1Clicked: {console.log("cliked, setting db"); landmarkModel.setDbFileName("landmarks.db") }
        onButton2Clicked: {console.log('cliked, setting vis false'); circle_basic_view.visible = false}
        onButton3Clicked: {console.log('cliked, setting vis true'); circle_basic_view.visible = true}
    }
    //TestCase {
    //    name: "MapObjectView"
        //when: windowShown

    //    function test_basics() {
     //       landmarkModel.setDbFileName("landmarks.db")
     //       tryCompare(landmarkModel, "count", 7, 2000)
      //  }
    //}
}
