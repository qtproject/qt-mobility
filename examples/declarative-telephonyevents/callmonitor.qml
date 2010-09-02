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
***********************************/

import Qt 4.7
import QtMobility.telephony 1.1
import "content"

//Main window
Rectangle {
    width: 640
    height: 480
    id: screen
    color: "#000000"
    gradient: Gradient {
         GradientStop { position: 0.0; color: "#000000" }
         GradientStop { position: 0.5; color: "#888888" }
         GradientStop { position: 1.0; color: "#000000" }
     }
    
    //Setting title
    Text {
        id: title
        text: "Call Monitor"
        color: "orange"
        font.bold: true
        font.pointSize: 24
    }
    
    //Assign Telephony Events signals to slots
    TelephonyCallList {
        id: telephonycallist
        onEvActiveCallAdded: screen.evAddItem(call, "call added")
        onEvActiveCallStatusChanged: screen.evAddItem(call, "status changed")
        onEvActiveCallRemoved: screen.evAddItem(call, "call removed")
    }

    //Adds a new call item into the list
    function evAddItem(call, eventstr)
    {

        console.log(call.remotePartyIdentifier)
        console.log(call.status)
        var statusstr = "";
        var typestr = "";

        //Convert status value to a string
        switch(call.status){
          case 0:
                statusstr = "Idle";
                break;
          case 1:
                statusstr = "Dialing";
                break;
          case 2:
                statusstr = "Alerting";
                break;
          case 3:
                statusstr = "Connected";
                break;
          case 4:
                statusstr = "Disconnecting";
                break;
          case 5:
                statusstr = "Incomming";
                break;
          case 6:
                statusstr = "OnHold";
                break;
          default:
                statusstr = "Unknown";
        }
        
        //Convert type value to a string
        switch(call.type){
          case 0:
                typestr = "Any";
                break;
          case 1:
                typestr = "Text";
                break;
          case 2:
                typestr = "Data";
                break;
          case 3:
                typestr = "Video";
                break;
          case 4:
                typestr = "Other";
                break;
          case 5:
                typestr = "Other";
                break;
          default:
                typestr = "Unknown";
        }
        
        //Add new call into the list model
        callModel.append({
                    "name": call.remotePartyIdentifier,
                    "status": statusstr,
                    "event": eventstr,
                    "type": typestr
                })
    }
    
    ListModel {
        id: callModel

        ListElement {
            name: "RemoteID"
            status: "Status"
            event: "Event"
            type: "Type"
        }
    }
    
    
    // The delegate for each fruit in the model:
    Component {
        id: listDelegate
        
        Item {
            id: delegateItem
            width: listView.width; height: 55
            clip: true

            //Row design for a list item
            Row {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    Text { 
                        text: name
                        font.pixelSize: 15
                        color: "#FF9900"
                        font.bold: true
                    }
                }
                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    Text { 
                        text: event
                        font.pixelSize: 15
                        color: "#FFBB00"
                    }
                }
                Column {
                    anchors.verticalCenter: parent.verticalCenter

                    Text { 
                        text: "-" + status
                        font.pixelSize: 15
                        color: "#FFDD00"
                    }
                }
                
                Column {
                    anchors.verticalCenter: parent.verticalCenter

                    Text { 
                        text: type + " call"
                        font.pixelSize: 15
                        color: "#FFFF00"
                    }
                }
            }

            // Animate adding and removing of items:
            ListView.onAdd: SequentialAnimation {
                PropertyAction { target: delegateItem; property: "height"; value: 0 }
                NumberAnimation { target: delegateItem; property: "height"; to: 55; duration: 250; easing.type: Easing.InOutQuad }
            }

            ListView.onRemove: SequentialAnimation {
                PropertyAction { target: delegateItem; property: "ListView.delayRemove"; value: true }
                NumberAnimation { target: delegateItem; property: "height"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
                // Make sure delayRemove is set back to false so that the item can be destroyed
                PropertyAction { target: delegateItem; property: "ListView.delayRemove"; value: false }
            }
        }
    }


        // The view:
    ListView {
        id: listView
        anchors.fill: parent; anchors.margins: 20
        model: callModel
        delegate: listDelegate
    }
    
    Row {
        anchors { left: parent.left; bottom: parent.bottom; margins: 20 }
        spacing: 10

        TextButton { 
            text: "Remove all items" 
            onClicked: callModel.clear()
        }
    }
}


