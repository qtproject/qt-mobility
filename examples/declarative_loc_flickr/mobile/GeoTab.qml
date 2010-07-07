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
import QtMobility.location 1.0


Rectangle {
    id: container
    property int maxX: parent.width; property int maxY: parent.height
    property alias latitude: position.latitude
    property alias longtitude: position.longtitude

    width: 200; height: 110
    color: "blue"
    opacity: 0.7
    border.color: "black"
    border.width: 1
    radius: 5
    gradient: Gradient {
        GradientStop {position: 0.0; color: "grey"}
        GradientStop {position: 1.0; color: "black"}
    }
    MouseArea {
        anchors.fill: parent
        drag.target: parent
        drag.axis: Drag.XandYAxis
        drag.minimumX: -(parent.width * (2/3)); drag.maximumX: parent.maxX - (parent.width/3)
        drag.minimumY: -(parent.height/2); drag.maximumY: parent.maxY - (parent.height/2)
    }
    Button {
        id: locateButton
        text: "Locate & update"
        anchors {left: parent.left; leftMargin: 5}
        y: 3; height: 32; width: parent.width - 10
        onClicked: {position.update();}
    }
    Position {
        id: position
        nmeaSource: "nmealog.txt"
        onPositionUpdated: {planet.source = "images/sun.png";}
    }
    function printableMethod(method) {
        if (method == Position.SatellitePositioningMethod)
            return "Satellite";
        else if (method == Position.NoPositioningMethod)
            return "Not available"
        else if (method == Position.NonSatellitePositioningMethod)
            return "Non-satellite"
        else if (method == Position.AllPositioningMethods)
            return "Multiple"
        return "source error";
    }

    Grid {
        id: locationGrid
        columns: 2
        anchors {left: parent.left; leftMargin: 5; top: locateButton.bottom; topMargin: 5}
        spacing: 5
        Text {color: "white"; font.bold: true
            text: "Lat:"; style: Text.Raised; styleColor: "black"
        }
        Text {id: latitudeValue; color: "white"; font.bold: true
            text: position.latitude; style: Text.Raised; styleColor: "black";
        }
        Text {color: "white"; font.bold: true
            text: "Lon:"; style: Text.Raised; styleColor: "black"
        }
        Text {id: longtitudeValue; color: "white"; font.bold: true
            text: position.longtitude; style: Text.Raised; styleColor: "black"
        }
    }
    Image {
        id: planet
        anchors {top: locationGrid.bottom; left: parent.left; leftMargin: locationGrid.anchors.leftMargin}
        source: "images/moon.png"
        width: 30; height: 30
    }
    Text {id: sourceText; color: "white"; font.bold: true;
        anchors {left: planet.right; leftMargin: 5; verticalCenter: planet.verticalCenter}
        text: "Source: " + printableMethod(position.positioningMethod); style: Text.Raised; styleColor: "black";
    }
}
