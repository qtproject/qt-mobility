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
// ![0]
import QtMobility.serviceframework 1.1
// ![0]

Rectangle {
    id: mainWindow

    property int size: 0
    property int curr: 0
    property double screenScale: 1
    property string searchText: ""
    property bool validService: false
    property alias screenWidth: mainWindow.width
    property alias screenHeight: mainWindow.height
    property alias screenColor: mainWindow.color

    // ![1]
    property variant notesManager: 0
    // ![1]

    SystemPalette { id: activePalette; colorGroup: SystemPalette.Active }

    Text {
        id: title
        text: "ToDoTool"
        font.pointSize: 24 * screenScale; font.family: "Nimbus Roman No9 L"; font.bold: true; font.italic:true 
        color: "blue"
        y: 5; 
        anchors.horizontalCenter: mainWindow.horizontalCenter
    }

    Button {
        id: addButton
        image: "icons/addIcon.png"
        width: mainWindow.width * 3/11
        height: mainWindow.height * 6/53
        anchors.right: deleteButton.left;
        anchors.top: title.bottom; anchors.topMargin: 10

        onClicked: {
            if (validService) {
                addDialog.opacity = 1;
                enableScreen(false);
            }
        }
    }

    Button {
        id: deleteButton
        image: "icons/deleteIcon.png"
        width: mainWindow.width * 3/11
        height: mainWindow.height * 6/53
        anchors.horizontalCenter: mainWindow.horizontalCenter
        anchors.top: title.bottom; anchors.topMargin: 10

        onClicked: {
            if (validService && size > 0) { 
                deleteDialog.opacity = 1;
                enableScreen(false);
            }
        }
    }

    Button {
        id: searchButton
        image: "icons/searchIcon.png"
        width: mainWindow.width * 3/11
        height: mainWindow.height * 6/53
        anchors.left: deleteButton.right;
        anchors.top: title.bottom; anchors.topMargin: 10

        onClicked: {
            if (validService) {
                searchDialog.opacity = 1;
                enableScreen(false);
            }
        }
    }

    Button {
        id: nextButton
        image: "icons/nextIcon.png"
        width: mainWindow.width * 2/11;
        height: mainWindow.height * 6/53
        anchors.left: countLabel.right;
        anchors.top: deleteButton.bottom; anchors.topMargin: 10

        onClicked: { 
            if (validService && curr < size) { 
                curr++;
                refreshNotes();
            }
        }
    }

    Button {
        id: prevButton
        image: "icons/prevIcon.png"
        width: mainWindow.width * 2/11;
        height: mainWindow.height * 6/53
        anchors.right: countLabel.left;
        anchors.top: deleteButton.bottom; anchors.topMargin: 10

        onClicked: {
            if (validService && curr > 1) {
                curr--;
                refreshNotes();
            }
        }
    }

    Text {
        id: countLabel
        text: curr + "/" + size
        font.pointSize: 10 * screenScale
        color: activePalette.text
        width: mainWindow.width * 2/11;
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: mainWindow.horizontalCenter
        anchors.verticalCenter: prevButton.verticalCenter
    }

    Rectangle {
        id: datetimeArea
        width: mainWindow.width * 8/11
        height: mainWindow.height * 4/53
        color: "#FFFF7F"
        anchors.horizontalCenter: mainWindow.horizontalCenter
        anchors.top: prevButton.bottom; anchors.topMargin: 10
    }

    Rectangle {
        id: noteArea
        width: mainWindow.width * 8/11
        height: mainWindow.height * 22/53
        color: "#FFFF7F"
        anchors.horizontalCenter: mainWindow.horizontalCenter
        anchors.top: datetimeArea.bottom;
    }

    Text {
        id: datetimeLabel
        text: ""
        font.pointSize: 10 * screenScale
        anchors.right: datetimeArea.right
        anchors.top: datetimeArea.top
    }

    Text {
        id: noteLabel
        text: ""
        font.pointSize: 18 * screenScale; font.family: "Comic Sans MS"; font.italic:true
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.Wrap
        width: noteArea.width
        anchors.verticalCenter: noteArea.verticalCenter
        anchors.horizontalCenter: title.horizontalCenter
    }

    InputDialog {
        id: interfaceDialog
        text: "Specify Notes Manager Interface"
        defaultText: "com.nokia.qt.examples.NotesManager"
        cancelable: false
        opacity: 1

        onConfirmed: {
            interfaceDialog.defaultText = input;
            
            // ![2]
            notesService.interfaceName = input;

            if (notesService.valid) {
                notesManager = notesService.serviceObject;
                notesManager.init();
            // ![2]
                var list = notesManager.noteSet;
                if (list.length > 0) {curr = 1;}

                refreshNotes();
                validService = true;
                enableScreen(true);

            } else {
                invalidDialog.opacity = 1;
                noteLabel.text = "Notes Manager service not found";
            
                addButton.color = "gray";
                deleteButton.color = "gray";
                searchButton.color = "gray";
                nextButton.color = "gray";
                prevButton.color = "gray";
            }
        }

        Component.onCompleted: { enableScreen(false); }
    }

    InputDialog {
        id: addDialog
        text: "Add a new note + alarm of format:\nnote#yyyy-mm-dd#hh:mm"

        onConfirmed: {
            var note = input.split("#");

            if (note.length == 3) {
                var date = note[1].split("-");
                var time = note[2].split(":");

                if (date.length == 3 && time.length ==2) {
                    notesManager.addNote(note[0], note[1] + " " + note[2] + ":00");
                }
            } else {
                    notesManager.addNote(note[0], currentDateTime() + ":00");
            }

            refreshNotes();
        }
    }

    InputDialog {
        id: searchDialog
        text: "Find a note:"
        size: 100

        onConfirmed: {
            searchText = input;
            curr = 1; 
            refreshNotes()
        }
    }

    Dialog {
        id: deleteDialog
        text: "Confirm removing this note item?"

        onConfirmed: {
            // ![3]
            var list = notesManager.noteSet;
            notesManager.removeNote(list[curr-1].index);
            // ![3]

            if (curr > 1) { curr--; }

            refreshNotes();
        }
    }

    Dialog {
        id: alarmDialog
        text: "ALERT SOUNDED!!!"
        cancelable: false
    }

    Dialog {
        id: invalidDialog
        text: "No valid default interface for:\n\n\"" + interfaceDialog.defaultText + "\""
        cancelable: false
    }

    Dialog {
        id: unconnectedDialog
        text: "Unable to connect to notes manager database\n in current or home directory"
        cancelable: false
    }

    function refreshNotes()
    {
        // ![4]
        notesManager.setSearch(searchText);
        var list = notesManager.noteSet;
        size = list.length;
        
        // ![4]
       
        if (size < 1) curr = 0;
        else if (size > 0 && curr == 0) curr = 1;
      
        // ![5]
        if (size > 0) {
            noteLabel.text = list[curr-1].message;
            datetimeLabel.text = formatDateTime(list[curr-1].alarm);
        }
        // ![5]
        else {
            noteLabel.text = "Click + to add a new note";
            datetimeLabel.text = "";
        }
    }

    function formatDateTime(datetime)
    {
        var dt = new Date(datetime);

        var month = (dt.getMonth() + 1) + ""; 
        if (month.length == 1)  month = "0" + month;

        var date = dt.getDate() + ""; 
        if (date.length == 1)  date= "0" + date;
            
        var hour = dt.getHours() + ""; 
        if (hour.length == 1)  hour = "0" + hour;
            
        var mins = dt.getMinutes() + ""; 
        if (mins.length == 1)  mins = "0" + mins;

        return (dt.getFullYear() + "-" + month + "-" + date + " " + hour + ":" + mins);
    }

    function currentDateTime()
    {
        var dt = new Date();

        return formatDateTime(dt);
    }
    
    function enableScreen(enable)
    {
        addButton.enableButton = enable;
        deleteButton.enableButton = enable;
        searchButton.enableButton = enable;
        nextButton.enableButton = enable;
        prevButton.enableButton = enable;
    }

    // ![6]
    Service {
        id: notesService
        interfaceName: "com.nokia.qt.examples.NotesManager"

        Component.onCompleted: {
            notesManager = notesService.serviceObject;
        }
    }
    // ![6]

    // ![7]
    Connections {
        target: notesManager
        ignoreUnknownSignals: true

        onSoundAlarm: {
            alarmDialog.text = "ALERT SOUNDED!!!" + "\n\n" + 
                               formatDateTime(alarm) + "\n\n" + notesManager.alarmMessage;
            alarmDialog.opacity = 1;
        }
    }
    // ![7]
}
