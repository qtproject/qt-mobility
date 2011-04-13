/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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


import QtQuick 1.0
import QtMobility.connectivity 1.2

Image {
    id: connectButton

    property bool enabled: true
    property int roRotation
    property BluetoothDiscoveryModel mymodel
    property BluetoothSocket socket
    property BluetoothSocket server_socket

    source: "icons/connect.png"
    width: 100
    height: 100
    x: 440
    y: 220
    smooth: true
    opacity: 1.0

    NumberAnimation {
        id: connectRotation

        target:  connectButton
        property: "rotation"
        from: 0
        to: 360
        loops: -1
        duration: 1000
        running: mymodel.discovery

        onRunningChanged: if(!running) { connectStop.running = true; }

    }

    NumberAnimation {
        id: connectStop

        target: connectButton
        property: "rotation"
        loops: 1
        from: connectButton.roRotation
        to: 0
        duration: 1000*connectButton.roRotation/360
        running: false
    }

    onRotationChanged: if(!connectStop.running) roRotation = rotation;

    MouseArea {
        anchors.fill: connectButton
        enabled: connectButton.state != "disabled"
        hoverEnabled: true
        onClicked: {
            mymodel.discovery = !mymodel.discovery;
        }
    }

    states: [
        State {
            name: "disabled"
            when: socket.connected || server_socket.connected
            PropertyChanges { target: connectButton; opacity: 0.0; }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "disabled"
            reversible: true
            NumberAnimation { target: connectButton; property: "opacity"; duration: 750 }
        }
    ]

}
