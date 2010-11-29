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
import QtMobility.systeminfo 1.1
import "content"


Rectangle {
    id: screen
    color: "#343434"

    StorageInfo {
        id: storageinfo
        onLogicalDrivesChanged: updateList;

    }

    function getTotalSizeText(name) {
        var totalSpace = storageinfo.totalDiskSpace(name);
        if(totalSpace/1000 < 1000) {
            return Math.round(totalSpace/1000)+" kb avail.";
        } else if(totalSpace/1000/1000 < 1000) {
            return Math.round(totalSpace/1000/1000)+" Mb avail.";
        } else if(totalSpace/1000/1000/1000 < 1000) {
            return Math.round(totalSpace/1000/1000/1000)+" Gb avail.";
        }
        return "";
    }

    function getAvailableSizeText(name) {
        var dspace = storageinfo.availableDiskSpace(name);
        if(dspace/1000 < 1000) {
            return Math.round(dspace/1000)+" kb / ";
        } else if(dspace/1000/1000 < 1000) {
            return Math.round(dspace/1000/1000)+" Mb / ";
        } else if(dspace/1000/1000/1000 < 1000) {
            return Math.round(dspace/1000/1000/1000)+" Gb / ";
        }
        return "";
    }

    function getPercent(name) {
        return Math.round( 100 - ((storageinfo.availableDiskSpace(name) / storageinfo.totalDiskSpace(name)) * 100))
    }

    property alias storageList: storageinfo.logicalDrives;

    function updateList() {
        driveList:storageList: storageinfo.logicalDrives;
    }

    Component {
        id: listItem

        Row {
            id:row
            spacing:  10
            ProgressBar {
                width: 120
                height: 25
                maxval: getPercent(name.text)
                value: getPercent(name.text)
                NumberAnimation on value { duration: 1500; from: 0; to: getPercent(name.text); loops: 1 }
                ColorAnimation on color { duration: 1500; from: "lightsteelblue"; to: "thistle"; loops:1}
                ColorAnimation on secondColor { duration: 1500; from: "steelblue"; to: "#CD96CD"; loops: 1 }
            }
            Text { id: name; text: modelData; color: "white";}
            Text { text: getAvailableSizeText(name.text) + getTotalSizeText(name.text); color: "white";}
        }
    }

    ListView {
        anchors.fill: parent
        model: storageList
        delegate: listItem
    }
}


