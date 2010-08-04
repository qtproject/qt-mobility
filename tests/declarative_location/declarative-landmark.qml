/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import "content" as Content
import Qt 4.7
import QtMobility.location 1.1



Rectangle {
    id: page
    width: 350
    height: 350
    color: "olive"
    
    Text {
        id: title
        text: "Simple landmark test app"
        font {pointSize: 12; bold: true}
    }
    
    PositionSource {
        id: positionSource
        updateInterval: 1000
    }
    Landmark {
        id: lm
        name: "just to see if this landmark element instantiates"
    }
    LandmarkCategory {
        id: lmcat
        name: "just to see if this categoary element instantiates"
    }
    LandmarkNameFilter {
	id: townFilter
	name: "Orgrimmar" // needs to be exact match
    }
    LandmarkProximityFilter {
	id: myPositionFilter
	longitude: positionSource.position.longitude
	latitude: positionSource.position.latitude
	radius: 100
    }
    LandmarkProximityFilter {
	id: existingCityPositionFilter
	longitude: 12
	latitude: 85
	radius: 100 // metres
    }
    // The model where data stems
    LandmarkSource {
        id: lmsource
        active: true
	nameFilter: townFilter
	//proximityFilter: myPositionFilter
	//proximityFilter: existingCityPositionFilter
        maxItems: 15
    }
    // The view which lays the overall view
    ListView {
        id: mainList
        model: lmsource
        anchors {top: title.bottom; left: title.left}
        width: parent.width; height: parent.height
        delegate: listdelegate
        highlightFollowsCurrentItem: false
        focus: true
        anchors.fill: parent
        keyNavigationWraps: true
    }
     // The delegate which determines how individual element is shown
    Component {
	id: listdelegate
	Item {
	    width: 200; height: 50
	    Text { id: nameField; text: name }
	    //Text { id: nameFiedld; text: lmsource.nameFilter.name }
	    Text { id: phoneField; text: "  tel:"  + phone; anchors.left: nameField.right }
	    Text { id: latitudeField; text: "  lat:"  + latitude;  anchors.left: phoneField.right }
	    Text { id: longitudeField; text: "  lon:"  + longitude;  anchors.left: latitudeField.right }
	}
    }

    /*
    Column {
        id: data
        anchors {top: title.bottom; left: title.left}
        Text {text: "<==== Landmark ====>"}
        Text {text: "name: " + lm.name}
        Text {text: "<==== Landmark category ====>"}
        Text {text: "name: " + lmcat.name}
        Text {text: "<==== LandmarkSource ====>"}
        Text {text: "name: " + lmsource.name}
        Text {text: "active: " + lmsource.active}
        
    }
    
    Grid {
        id: actionRectangles
        x: 4;
        width: 150
        anchors { topMargin: 5; top: data.bottom}
        rows: 3; columns: 1; spacing: anchors.bottomMargin;
        Content.Cell {action: "unassigned 1"; onClicked: doAction(action)}
        Content.Cell {action: "unassigned 2"; onClicked: doAction(action)}
        Content.Cell {action: "unassigned 3";  onClicked: doAction(action)}
    }
    function doAction(action) {
        if (action == "unassigned 1") {
            positionSource.update()
        }
        else if (action == "unassigned 2" ) {
            positionSource.start();
        }
        else if (action == "unassigned 3" ) {
            positionSource.stop();
        }
    }
    */
}
