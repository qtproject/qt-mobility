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
import Controller 1.0

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
                socket.setService(s);
                socket.setConnected(true);
            }

            discovery: false
            minimalDiscovery: true
            //onDiscoveryChanged: console.log("Discovery: " + discovery)
            onNewServiceDiscovered: { connect(service); discovery = false; }
            uuidFilter: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
       }

//       BluetoothService {
//            id: btservice

//            deviceAddress: "00:1A:9F:92:9E:5A"
//            serviceUuid: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
//       }

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

           //connected: true
//           service: btservice

           onStateChanged: console.log("New socket state " + state);
           onErrorChanged: { statusText.text = error; }
           onServiceChanged: console.log("New service");
           onConnectedChanged: {
               console.log("Connected: " + connected);
               if(connected) {
                    statusText.text = "Connected";
                   controller.stop();
               }
               else {
                   statusText.text = "Unconnected " + error;
                   controller.start();
                   controller.refresh();
               }
           }
           //onConnectedChanged: console.log("Connected changed");
           onDataAvailable: parse(socket.stringData);
       }

       Timer {
           id: lagTimer

           function sendEcho() {
               if(socket.connected){
                    var s = "E " + new Date().getTime();
                    socket.sendStringData(s);
               }
               if(server_socket.connected){
                   var s = "e " + new Date().getTime();
                   server_socket.sendStringData(s);
               }
           }

           interval: 1000
           repeat: true
           running: socket.connected || server_socket.connected
           onTriggered: sendEcho();
       }


       // server side, not implemented

       BluetoothService {
            id: btserver

            serviceName: "bttennis"
            serviceDescription: "QML Tennis Service"
            serviceProtocol: "l2cap"
            serviceUuid: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
            registered: true
            onNewClient: {
                console.log("Got a new connections!");
                if (!server_socket.connected) {
                    assignNextClient(server_socket);
                } else {
                    nextClient().connected = false;
                }
            }
       }

       BluetoothSocket {
           id: server_socket

           function parse(s){
               var args  = s.split(" ");

               if(args[0] == "r"){
                   rightPaddle.y = Number(args[1])+topBumper.height;
               }
               else if(args[0] == "E"){ // echo packet, check for RTT
                   sendStringData(s);
               }
               else if(args[0] == "E"){
                   var d = new Date();

                   var lag = d.getTime() - args[1];
                   if(lag > 250){
                       statusText.text = "LAG! " + lag + "ms";
                   }
               }
           }

           onStateChanged: console.log("SERVER: New socket state " + state);
           onErrorChanged: { console.log("SERVER: error: " + error); statusText.text = error; }
           onServiceChanged: console.log("SERVER: New service");
           onConnectedChanged: {
               console.log("SERVER Connected: " + connected);
               if(connected) {
                    statusText.text = "Server Connected";
                    controller.refresh();
               }
               else {
                   statusText.text = "Server Unconnected " + error;
               }
           }
           onDataAvailable: parse(stringData);
       }

       Connections {
           target: deviceSensor.item
           onReadingChanged: {
                if (x >= 20 && x <= 70)
                    rightPaddle.paddlePos = ((x - 20) / 50 * page.height) - rightPaddle.height / 2;
           }
       }

       Handover {
           id: handover

           bluetoothService: BluetoothService {
               serviceName: "bttennis"
               serviceProtocol: "l2cap";
               serviceUuid: "e8e10f95-1a70-4b27-9ccf-02010264e9c9"
           }

           onBluetoothServiceChanged: {
               console.log("Connecting to NFC provided address " + bluetoothService.deviceAddress);
               statusText.text = "Connecting: " + bluetoothService.deviceAddress + " " + bluetoothService.serverPort;
               socket.setService(bluetoothService);
               socket.setConnected(true);
           }

           onAvailableChanged: statusText.text = "Touch to play";
       }

       MouseArea {
           anchors.fill: parent
           enabled:  true

           onMousePositionChanged: {
               var y = mouseY-rightPaddle.height/2;
               var p = server_socket.connected ? leftPaddle : rightPaddle;

               if(y < topBumper.height)
                   y = topBumper.height;
               if(y > page.height - bottomBumper.height - rightPaddle.height)
                   y = page.height - bottomBumper.height - rightPaddle.height;
               p.paddlePos = y;
           }
       }

       ConnectButton {
           id: connectButton

           mymodel: myModel
           socket: socket
           server_socket: server_socket
       }

        // Make a ball to bounce
        Rectangle {
            id: ball

            x: page.width/2-width/2;
            y: page.height/2-height/2;
            width: 12; height: 12; z: 1
            color: fg
        }

        Paddle {
            id: leftPaddle

            property int lastsent: 0

            onPaddleMoved: {
                controller.moveLeftPaddle(y);
                if (server_socket.connected) {
                    var c = Math.round(y-topBumper.height);
                    if (c == lastsent){
                        return;
                    }
                    lastsent = c;
                    server_socket.sendStringData("l " + c);
                }
            }
        }

        Paddle {
            id: rightPaddle

            x: page.width - 12;

            onPaddleMoved:  {
                if(socket.connected)
                    socket.sendStringData("r " + Math.round(rightPaddle.y-topBumper.height));
                controller.moveRightPaddle(y);
            }

        }
        Bumper {
            id: topBumper
        }
        Bumper {
            id: bottomBumper
            y: page.height-height
        }
        Score {
            id: scoreLeft
        }
        Score {
            id: scoreRight
            x: 3*page.width/4-paintedWidth/2
        }

        Repeater {
            model: page.height / 8
            Rectangle { color: fg; x: page.width/2; y: index * 8; width: 2; height: 5 }
        }

        Text {
            id: statusText
            font.family: "Old English"
            font.pixelSize: 25; font.bold: true
            text: ""
            color:  fg
            y: page.height-height-25
            x: 24

            onTextChanged: NumberAnimation { target: statusText; property: "opacity"; easing.type: Easing.InOutSine; from: 1; to: 0; duration: 2000 }
        }

        Keys.onPressed: {
            var p = server_socket.connected ? leftPaddle : rightPaddle;

            if(event.key == Qt.Key_Up){
                p.paddlePos -= 20;
                event.accepted = true;
            }
            else if(event.key == Qt.Key_Down){
                p.paddlePos += 20;
                event.accepted = true;
            }
        }

        Loader {
            id: deviceSensor
            source: "sensor.qml"
        }

        transform: Scale { xScale: bounds.width/page.width; yScale: bounds.height/page.height }

        Controller {
            id: controller

            property bool dir: true;
            property int lastx: page.width/2;

            onMoveBall: {
                ball.x = x-ball.width/2;
                ball.y = y-ball.height/2;
                if((ball.x+ball.width > rightPaddle.x) &&
                   (ball.y+ball.width > rightPaddle.y) &&
                   (ball.y < rightPaddle.y + rightPaddle.height)){
                    ballCollision(4);

                }
                else if((ball.x < leftPaddle.width) &&
                        (ball.y+ball.width > leftPaddle.y) &&
                        (ball.y < leftPaddle.y + leftPaddle.height)){
                    ballCollision(3);

                }
                else if(ball.x > page.width){
                    scored(4);
                }
                else if(x < 0){
                    scored(3);
                }
                else if(ball.y < topBumper.height){
                    ballCollision(1);
                }
                else if((y + ball.height/2) > bottomBumper.y){
                    ballCollision(2);
                }
                if(lastx > x){
                    dir = true;
                }
                else {
                    dir = false;
                }
                lastx = x;

                if (!socket.connected && !server_socket.connected){
                    if(dir && ball.x < page.width/2) {
                        leftPaddle.paddlePos = ball.y - leftPaddle.height/2;
                    }
                    else if(!dir && ball.x > page.width/2){
                        rightPaddle.paddlePos = ball.y - leftPaddle.height/2;
                    }
                }

                if (server_socket.connected){
                    var s = "m " + ball.x + " " + ball.y;
                    server_socket.sendStringData(s);
                }
            }
            onScore: {
                scoreLeft.text = left;
                scoreRight.text = right;

                if (server_socket.connected) {
                    var s = "s " + left + " " + right;
                    server_socket.sendStringData(s);
                }
            }
        }
    }
}
