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
import QtMobility.serviceframework 1.0
import "content"

//Layout of the mainPage
//----------------------------------------------  ____ mainPage
//| ------------------- ---------------------- | /
//| | dialerList     | | dialScreen         | |/
//| |                 | |                    | |
//| |                 | |                    | |
//| |                 | |                    | |
//| ------------------- |                    | |
//| ------------------- |                    | |
//| | serviceDetails  | |                    | |
//| ------------------- |                    | |
//|                     |                    | |
//|                     |                    | |
//|                     |                    | |
//|                     |                    | |
//| ------------------- |                    | |
//| | status          | |                    | |
//| ------------------- ---------------------- |
//----------------------------------------------

Rectangle {
    property variant dialerObject: defaultService.serviceObject
    
    id: mainPage
    width: 500
    height: 250
    color: "white"

    DialerList {
        id: dialerList
        height: childrenRect.height + 10
        width: childrenRect.width
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: dialScreen.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        radius: 5
        color: "steelblue"
        border.color: "black"
        border.width: 3
        gradient:     
            Gradient {
                GradientStop {
                    position: 0.0
                    color: "lightsteelblue"
                }
                
                GradientStop {
                    position: 1.0
                    color: "steelblue"
                }
            }
        onSignalSelected: { serviceSelected(); }
    }

    function serviceSelected()
    {
        dialerObject = dialerList.dialService.serviceObject

        serviceDetails.text = "Selected dial service:" + "\n   " + 
                               dialerList.dialService.serviceName + 
                               "\n   (" + dialerList.dialService.versionNumber + ")";
    }
    
    Text {
        id: serviceDetails
        text: "Selected dial service:"
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5;
        anchors.left: parent.left
        anchors.top: dialerList.bottom
    }
    
    Text {
        id: status
        anchors.top: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: -40
        anchors.leftMargin: 5
    }
    
    Timer {
        id: clearStatusTimer
        interval: 2000
        running: false
        repeat: false
        onTriggered: {
            status.text = ""
        }
    }

    //! [0]
    DialScreen {
        id: dialScreen
        property bool activeCall : false
        property variant currentDialer: 0;
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        anchors.right: parent.right
        anchors.top: parent.top
        onDial: {
            if (activeCall == false) {
                if (dialerList.dialService != 0) {
                    var o = dialerObject;
                    status.text = "Dialing " + numberToDial +"...";
                    dialScreen.currentDialer = o;
                    o.dialNumber(numberToDial);
                    activeCall = true;
                }
            }
        }
        onHangup: {
            if (activeCall) {
                if (dialScreen.currentDialer != 0) {
                    dialScreen.currentDialer.hangup();
                }
                status.text = "Hang up";
            }
        }
    }
    //! [0]

    //! [1]
    Connections {
        target: dialerObject
        
        onStateChanged: {
            if (dialScreen.currentDialer.state == 1) {
                status.text += "\nRinging";
            } 
            else if (dialScreen.currentDialer.state == 2) {
                status.text += "\nConnected";
            } 
            else if (dialScreen.currentDialer.state == 0) {
                status.text += "\nConnection terminated";
                dialScreen.activeCall = false;
                clearStatusTimer.running = true;
            } 
            else if (dialScreen.currentDialer.state == 3) {
                status.text += "\nPhone already engaged";
            }
        }
    }
    //! [1]

    Service {
        id: defaultService
        interfaceName: "com.nokia.qt.examples.Dialer"
    }
}
