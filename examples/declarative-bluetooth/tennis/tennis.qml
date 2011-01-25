/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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
import QtMobility.connectivity 1.2

Rectangle {
    id: bounds

    property BluetoothService currentService
    property alias minimalDiscovery: myModel.minimalDiscovery
    property alias uuidFilder: myModel.uuidFilter
    property color fg: "white"
    property color bg: "black"

    Rectangle {
        id: page

        width: 640
        height: 360
        color: bg
        focus:  true

        // client side

        BluetoothDiscoveryModel {
            id: myModel

            function connect(s){
                console.log("Found new service " + s.deviceAddress + " " + s.deviceName + " " + s.serviceName);
                socket.setService(s);
                socket.setConnected(true);
            }

            discovery: false
            minimalDiscovery: false
            onDiscoveryChanged: console.log("Discovery done")
            onNewServiceDiscovered: connect(service);
            uuidFilter: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
       }

       BluetoothService {
            id: btservice

            //deviceAddress: "00:21:86:E8:0F:8D"
            deviceAddress: "00:1A:9F:92:9E:5A"
            serviceUuid: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
       }

       BluetoothSocket {
           id: socket

           function parse(s){
               var args  = s.split(" ");

               if(args[0] == "m"){
                   ball.x = args[1]-ball.width/2;
                   ball.y = args[2]-ball.height/2;
               }
               else if(args[0] == "l"){
                   leftPaddle.y = Number(args[1])+topBumper.height;
               }
               else if(args[0] == "s"){
                   scoreLeft.text = args[1];
                   scoreRight.text = args[2];
               }
               else if(args[0] == "e"){ // echo packet, check for RTT
                   socket.sendStringData(s);
               }
               else if(args[0] == "E"){
                   var d = new Date();

                   var lag = d.getTime() - args[1];
                   if(lag > 250){
                       statusText.text = "LAG! " + lag + "ms";
                   }
               }
           }

           connected: true
           service: btservice

           onStateChanged: console.log("New socket state " + state);
           onErrorChanged: { statusText.text = error; reconnect.start(); }
           onServiceChanged: console.log("New service");
           onConnectedChanged: {
               if(connected) {
                    statusText.text = "Connected";
               }
               else {
                   statusText.text = "Unconnected " + error;
       }
           }
           //onConnectedChanged: console.log("Connected changed");
           onDataAvailable: parse(socket.stringData);
       }

       Timer {
           id: reconnect
           interval: 15000
           onTriggered: socket.connected = true;
       }

       Timer {
           id: lagTimer

           function sendEcho() {
               var s = "E " + new Date().getTime();
               socket.sendStringData(s);
//               console.log(s);
           }

           interval: 1000
           repeat: true
           running: socket.connected
           onTriggered: sendEcho();
       }


       // server side, not implemented

    //   BluetoothService {
    //        id: btserver

    //        serviceName: "bttennis"
    //        serviceDescription: "QML Tennis Service"
    //        serviceProtocol: "l2cap"
    //        serviceUuid: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
    //        registered: true
    //   }

    //   BluetoothSocket {
    //       id: socket_listen

    //       service: btserver
    //       listening: true
    //   }


       Keys.onPressed: {
           if(event.key == Qt.Key_Up){
               rightPaddle.y -= 20;
               event.accepted = true;
           }
           else if(event.key == Qt.Key_Down){
               rightPaddle.y += 20;
               event.accepted = true;
           }
       }

       Loader {
           id: deviceSensor
           source: "sensor.qml"
       }

       Connections {
           target: deviceSensor.item
           onReadingChanged: {
                if (x >= 20 && x <= 70)
                    rightPaddle.y = ((x - 20) / 50 * page.height) - rightPaddle.height / 2;
           }
       }


       MouseArea {
           anchors.fill: parent
           enabled:  true
           hoverEnabled: true

           onMousePositionChanged: {
               var y = mouseY-rightPaddle.height/2;
               if(y < topBumper.height)
                   y = topBumper.height;
               if(y > page.height - bottomBumer.height - rightPaddle.height)
                   y = page.height - bottomBumer.height - rightPaddle.height;
               rightPaddle.y = y;
           }
       }


        // Make a ball to bounce
        Rectangle {
            id: ball

            x: page.width/2-width/2; width: 12; height: 12; z: 1
            color: fg
        }

        Rectangle {
            id: leftPaddle
            color: fg
            x: 0; width: 12; height: 50; y: page.height/2-height/2;
        }
        Rectangle {
            id: rightPaddle
            color: fg
            x: page.width - 12; width: 12; height: 50
            y: page.height/2-height/2
            Behavior on y { SpringAnimation{ damping: 0.2; spring: 4.5; velocity: 300 } }
            onYChanged: {
                if(y < topBumper.height)
                    y = topBumper.height;
                if(y > page.height-bottomBumer.height-height)
                    y = page.height-bottomBumer.height-height;
                if(socket.state == "Connected") {
                    socket.sendStringData("r " + Math.round(rightPaddle.y-topBumper.height));
                }
            }
        }
        Rectangle {
            id: topBumper
            color: fg
            x: 0; y: 0
            height: 12; width: page.width
        }
        Rectangle {
            id: bottomBumer
            color: fg
            x: 0; y: page.height-height
            height: 12; width: page.width
        }
        Text {
            id: scoreLeft
            font.family: "Old English"
            font.pixelSize: 50; font.bold: true
            color: fg
            text:  "0"
            y: 50
            x: page.width/4-paintedWidth/2
        }
        Text {
            id: scoreRight
            font: scoreLeft.font
            text:  "0"
            color: fg
            y: 50
            x: 3*page.width/4-paintedWidth/2
        }

        // The rest, to make it look realistic, if neither ever scores...
        Repeater {
            model: page.height / 8
            Rectangle { color: fg; x: page.width/2; y: index * 8; width: 2; height: 5 }
        }


        Text {
            id: statusText
            font.family: "Old English"
            font.pixelSize: 25; font.bold: true
            text:  ""
            color:  fg
            y: page.height-height-25
            x: 24

            onTextChanged: NumberAnimation { target: statusText; property: "opacity"; easing.type: Easing.InOutSine; from: 1; to: 0; duration: 2000 }
        }

        transform: Scale { xScale: bounds.width/page.width; yScale: bounds.height/page.height }

    }

}
