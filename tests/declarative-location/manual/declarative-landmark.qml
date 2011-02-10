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
    width: 800
    height: 350
    color: "olive"
    
    Text { id: title;  text: "Simple landmark test app"; font {pointSize: 12; bold: true}}
    
    // Just to see everything instantiates, also used in docs.

    //![User declared landmark]
    Landmark { 
        id: myLandmark
	name: "Tangalooma wrecks"
	coordinate: Coordinate {
            latitude: -27.163238
            longitude: 153.368373
	}
    }
    //![User declared landmark]

    //![User declared category]
    LandmarkCategory {
        id: myCategory
        name: "Best dive sites"
    }
    //![User declared category]

    LandmarkNameFilter { }
    LandmarkProximityFilter { }
    LandmarkIntersectionFilter { }
    LandmarkUnionFilter { }

    // Example: landmark name filter
    LandmarkNameFilter {
	id: landmarkFilterUndercity
	name: "Undercity"
    }    
    // Example: landmark proximity filter
    LandmarkProximityFilter {
        id: landmarkFilterSpecificLocation
	center: Coordinate {
	        longitude: 10
		latitude: 20
        }
	radius: 500
    }

    PositionSource {
        id: myPosition
        updateInterval: 1000
    }
    LandmarkProximityFilter {
        id: landmarkFilterMyCurrentLocation
	center: myPosition.position.coordinate
	radius: 500
    }
    
    // Example: landmark intersection filter
    LandmarkIntersectionFilter {
        id: landmarkIntersectionFilterNameAndProximity
	LandmarkNameFilter {
	    name: "Darwin"
        }
	LandmarkProximityFilter {
	    center: Coordinate {
	        longitude:  10
	        latitude: 20
		}
	        radius: 500
        }
    }

    
    // Example: landmark union filter
    LandmarkUnionFilter {
        id: landmarkUnionFilterNameAndProximity
	LandmarkNameFilter {
	    name: "Nimbin"
        }
	LandmarkProximityFilter {
            center: myPosition.position.coordinate
	    radius: 50
	}
    }
    
    // Example: landmark model
    LandmarkModel {
        id: landmarkModel
	//  autoUpdate determines if model is automatically updated:
	//  1) after instantiated  (no need to call update())
	//  2) whenever database data changes (QLandmarkManager reports changes)
	//  3) filters are changed
	// Value is true by default
        autoUpdate: false
	// filter determines what data is requested
	//filter: landmarkFilterUndercity
	
	// Imports landmarks and categories from files.
	// Setting this variable automatically triggers the import operation,
	// irrespective of ongoing updates (or autoUpdate) setting.
	// importFile: "AUS-PublicToilet-Queensland.gpx"
	// If seen convinient, the landmarks can be enumerated like this as well, instead of 
	// accessing them via model
	onLandmarksChanged: {
	    console.log("log: Landmark count is: "+ count);
	    for (var index = 0; index < landmarks.length; index++)  {
	        console.log("Index, name:" + index + " , " + landmarks[index].name);
            }
	}
	
	/* 
	// Alternatively filter could be declared directly
	filter: LandmarkIntersectionFilter {
	    LandmarkNameFilter {
	        ...
	    }
	    LandmarkUnionFilter {
	        LandmarkIntersectionFilter {
		    LandmarkNameFilter {
		        ...
		    }
		    LandmarkProximityFilter {
		        ...
		    }
		}
	    }
	}
	*/
	// Limit determines the maximum number of items this model holds
        limit: 100
	// Offset determines the index where to start elements (e.g. in tabbed/paged browsing)
	offset: 0
    }

    // Example: landmark category model

    //![Categories list iteration]
    LandmarkCategoryModel {
        id: categoriesOfGivenLandmark
	autoUpdate: false
        onCategoriesChanged: {
            console.log("log: Categories count is: "+ count);
            for (var index = 0; index < categories.length; index++)  {
                console.log("Index, name:" + index + " , " + categories[index].name);
            }
        }
    }
    //![Category list iteration]
    
    // The view
    ListView {
        id: mainList
        model: landmarkModel
        delegate: landmarklistdelegate
        anchors {top:title.bottom;}
        width: parent.width; height: parent.height
        highlightFollowsCurrentItem: false
        focus: true
        anchors.fill: parent
        keyNavigationWraps: true
    }
    
     // The delegate which determines how individual element is shown
    Component {
	id: landmarklistdelegate
	Item {
	        width: 200; height: 20
                Text {
                    id: nameField; text: landmark.name
                    MouseArea{
                        anchors.fill: parent
                        onClicked: categoriesOfGivenLandmark.landmark = landmark
                    }
                }
                Text { id: phoneField; text: "  tel:"  + landmark.phoneNumber; anchors.left: nameField.right;}
	        Text { id: latitudeField; text: "  lat:"  + landmark.coordinate.latitude;  anchors.left: phoneField.right }
	        Text { id: longitudeField; text: "  lon:"  + landmark.coordinate.longitude;  anchors.left: latitudeField.right }
		}
        }

    Component {
	id: categorylistdelegate
	Item {
	    width: 200; height: 50
	    Text { id: nameField; text: name }
	}
    }

    Grid {
        id: actionRectangles
        x: 4;
        width: 150
        anchors { topMargin: 5; top: title.bottom; right: page.right}
        //rows: 3; 
	columns: 1; spacing: anchors.bottomMargin;
        Content.Cell {action: "Change DB"; onClicked: doAction(action)}
        Content.Cell {action: "Import"; onClicked: doAction(action)}
        Content.Cell {action: "Update"; onClicked: doAction(action)}
        Content.Cell {action: "Set import file"; onClicked: doAction(action)}
        Content.Cell {action: "Autoupdate";  onClicked: doAction(action)}
    }
    function doAction(action) {
        if (action == "Change DB") {
	    console.log("log: Changing DB to generatedExampleLandmarkDb.db");
            landmarkModel.setDbFileName("generatedExampleLandmarkDb.db");
        }
        else if (action == "Set import file" ) {
	    console.log("log: Setting import file to mylm.lmx")
	    landmarkModel.importFile = "mylm.lmx"
        }
        else if (action == "Import" ) {
	    console.log("log:Calling importLandmarks()")
	    landmarkModel.importLandmarks()
        }
        else if (action == "Update" ) {
	    console.log("log: Calling update()")
	    landmarkModel.update()
	}
	else if (action == "Autoupdate" ) {
	    if (landmarkModel.autoUpdate == true) {
		console.log("log: setting autoUpdate false")
		landmarkModel.autoUpdate = false;
	    } else {
		console.log("log: setting autoUpdate true")
		landmarkModel.autoUpdate = true;
	    }
	} else {
            console.log("log: Unassigned clicked")
        }
    }
}
