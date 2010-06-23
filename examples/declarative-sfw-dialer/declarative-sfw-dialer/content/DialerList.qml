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

import Qt 4.7
// ![4]
import QtMobility.serviceframework 1.0
// ![4]

//Layout of the ServiceList control
//---------------------------------
//|ServiceList                    |
//| ----------------------------- |
//| |title                      | |
//| ----------------------------- |
//| ----------------------------- |
//| |listFrame                  | |
//| |-------------------------- | |
//| ||serviceListView         | | |
//| ||- listItem              | | |
//| ||- listItem              | | |
//| ||- listItem              | | |
//| |---------------------------| |
//| ----------------------------- |
//---------------------------------
    
Rectangle {
    property variant dialService: 0
    signal signalSelected
    
    Text {
        id: title
        height: 20
        width: 200
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "<b>Select dial service:</b>"
    }
    
    Rectangle {
        id : listFrame
        width: childrenRect.width
        height: childrenRect.height
        anchors.top: title.bottom
        anchors.left: parent.left;
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        border.color: "black"
        border.width: 3
        property bool nohighlightlistitem : true
        //! [1]
        Component {
            id: delegate
            //! [1]
            //Rectangle item to draw a list view item
            //This includes 2 line of text:
            // ------------------------------------------
            // |Service: LandDialer (1.0)               |
            // |Interface: com.nokia.qt.examples Dialer |
            // ------------------------------------------
            Rectangle {
                id: listItem
                width: serviceListView.width
                height: 40
                border.color: "black"
                border.width: 1
                opacity: 0.6

                //! [2]
                MouseArea {
                    id: listItemMouseRegion
                    anchors.fill: parent
                    onClicked: {
                        if(listFrame.nohighlightlistitem){
                            serviceListView.highlight = highlight
                            listFrame.nohighlightlistitem = false;
                        }
                        serviceListView.currentIndex = index;
                        dialService = model.modelData;
                        signalSelected()
                    }
                }

                Text { 
                    id: serviceItemInfo
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.topMargin: 5
                    anchors.leftMargin: 3
                    text: " <b>Service:</b> " + serviceName + "  (" + versionNumber + ")"
                }
                
                Text { 
                    id: serviceItemInterfaceName
                    anchors.top: serviceItemInfo.bottom
                    anchors.left: parent.left
                    anchors.topMargin: 2
                    anchors.leftMargin: 3
                    text: " <b>Interface:</b> " + interfaceName;
                }
                //! [2]
            }
        }
        
        //! [3]
        Component {
            id: highlight
            
            Rectangle {
                width: childrenRect.width
                border.color: "black"; border.width: 2
                height: 30
                color : "lightsteelblue"
                gradient: Gradient {
                    GradientStop {position: 0.0; color: "steelblue"}
                    GradientStop {position: 0.5; color: "lightsteelblue"}
                    GradientStop {position: 1.0; color: "steelblue"}
                }
            }
        }
        //! [3]
        
        //! [0]
        ListView {
            id: serviceListView
            height: 100
            width: 260
            anchors.topMargin: 5
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            model: dialerServiceList.services
            opacity: 1
            delegate: delegate
            currentIndex: -1
            clip: true
        }
        //! [0]
    }

    //! [5]
    ServiceList {
        id: dialerServiceList
        interfaceName: "com.nokia.qt.examples.Dialer"
        minVersion: "1.0"
    }
    //! [5]
}
