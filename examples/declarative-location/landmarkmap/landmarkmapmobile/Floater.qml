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


Rectangle {
    id: container
    property int maxX: parent.width; property int maxY: parent.height
    property double latitude
    property double longitude
    property int landmarks
    property int categories

    width: 180; height: 145
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

    Column {
        id: locationGrid
        anchors {left: parent.left; leftMargin: 5; top: parent.top; topMargin: 5}
        spacing: 5
        Text {color: "white"; font.bold: true
            text: "My location:"; style: Text.Raised; styleColor: "black"
        }
        Text {color: "white"; font.bold: true; width: container.width - 5; elide: Text.ElideRight
            text: "Lat: " + latitude; style: Text.Raised; styleColor: "black"
        }
        Text {color: "white"; font.bold: true; width: container.width - 5; elide: Text.ElideRight
            text: "Lon: " + longitude; style: Text.Raised; styleColor: "black"
        }
        Text {color: "white"; font.bold: true
            text: "Landmarks: " + landmarks; style: Text.Raised; styleColor: "black"
        }
        Text {color: "white"; font.bold: true
            text: "Categories: " + categories; style: Text.Raised; styleColor: "black"
        }
    }
}
