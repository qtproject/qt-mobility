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
import QtMobility.systeminfo 1.0

Rectangle {
    width: 640
    height: 480
    color: "#343434"

    Item {
        id: wlan
        x:(parent.width / 2) - (parent.width / 4);
        y: parent.height / 2
        //        anchors { verticalCenter: parent.verticalCenter; }

        property int mode: SystemNetworkInfo.WlanMode;

        QDeclarativeNetworkInfo {
            id: wlaninfo
            currentMode: wlan.mode;
            property string img : getImage(networkStatus);

            function getImage(newStatus) {
                if(newStatus == "Connected") {
                    return "images/wlan.svg";
                }
                if(newStatus == "Searching") {
                    //                    return "images/wlan.svg";
                }
                if(newStatus == "No Network Available") {
                    return "images/wlan-noavail.svg";
                }
            }

            onStatusChanged : {
                img = getImage(newStatus)
            }
        }

        Component.onCompleted: {
            wlaninfo.startNameChanged();
            wlaninfo.startSignalStrengthChanged();
            wlaninfo.startStatusChanged();
            wlaninfo.startModeChanged();
        }

        Image {
            id: wifiimg
            source : wlaninfo.img;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;
        }

        Text {
            id: signalText
            color: "white"
            text: wlaninfo.networkSignalStrength
            anchors.top: wifiimg.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
        }


        Text {
            id: sigmstatus
            color: "white"
            text: wlaninfo.networkStatus;
            anchors.top: signalText.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
        }

        Text {
            id: signame
            color: "white"
            text:  wlaninfo.networkName;
            anchors.top: sigmstatus.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
        }

        //        Text {
        //            id: sigmac
        //            color: "white"
        //            text:  wlaninfo.macAddress;
        //            anchors.top: sigmstatus.bottom;
        //            anchors.horizontalCenter: parent.horizontalCenter;
        //        }
    }



    Item {
        id: ethernet
        x:(parent.width / 2) + (parent.width / 4);
        anchors { verticalCenter: parent.verticalCenter; }

        property string imageSrc;

        property int mode: SystemNetworkInfo.EthernetMode;

        QDeclarativeNetworkInfo {
            id: ethinfo
            currentMode: ethernet.mode;
            property string img : getImage(networkStatus);

            function getImage(newStatus) {
                if(newStatus == "Connected") {
                    return "images/lan.svg";
                }
                if(newStatus == "No Network Available") {
                    return "images/lan-noavail.svg";
                }
            }

            onStatusChanged : {
                img = getImage(newStatus)
            }
        }

        Component.onCompleted: {
            ethinfo.startNameChanged();
            ethinfo.startSignalStrengthChanged();
            ethinfo.startStatusChanged();
            ethinfo.startModeChanged();
        }

        Image {
            id: ether
            source :  ethinfo.img
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;
        }

        Text {
            id: ethersignalText
            color: "white"
            text: ethinfo.networkSignalStrength
            anchors.top: ether.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
        }

        Text {
            id: ethersigmstatus
            color: "white"
            text: ethinfo.networkStatus;
            anchors.top: ethersignalText.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
        }

        Text {
            id: ethersigname
            color: "white"
            text:  ethinfo.networkName;
            anchors.top: ethersigmstatus.bottom;
            anchors.horizontalCenter: parent.horizontalCenter;
        }


        //        Text {
        //            id: ethersigmac
        //            color: "white"
        //            text:  ethinfo.macAddress;
        //            anchors.top: ethersigmstatus.bottom;
        //            anchors.horizontalCenter: parent.horizontalCenter;
        //        }
    }

}
