import Qt 4.6
import QtSFW 1.0

Rectangle {
    property int size: 0
    property int curr: 0
    property string search: ""
    property variant notesObject: notesService.serviceObject()

    id: mainWindow
    color: "lightgray"
    width: 220; height: 265

    SystemPalette { id: activePalette }

    Rectangle {
        id: datetimeArea
        width: 160; height: 20
        x: 30; y: 120
        color: "#FFFF7F"
    }

    Rectangle {
        id: noteArea
        width: 160; height: 110
        x: 30; y: 140
        color: "#FFFF7F"
    }


    Text {
        id: title
        text: "ToDoTool"
        font.pointSize: 24; font.family: "Nimbus Roman No9 L"; font.bold: true; font.italic:true 
        color: "blue"
        y: 5; anchors.horizontalCenter: mainWindow.horizontalCenter
    }

    Text {
        id: countLabel
        text: curr + "/" + size
        font.pointSize:10
        y: 90
        anchors.horizontalCenter: mainWindow.horizontalCenter
    }

    Text {
        id: datetimeLabel
        text: ""
        font.pointSize:10
        x: 30; y: 120
        anchors.right: datetimeArea.right
    }

    Text {
        id: noteLabel
        text: "Click + to add a new note"
        font.pointSize: 18; font.family: "Comic Sans MS"; font.italic:true
        horizontalAlignment: Text.AlignHCenter
        wrap: true
        width: noteArea.width
        anchors.verticalCenter: noteArea.verticalCenter
        anchors.horizontalCenter: title.horizontalCenter
    }

    Button {
        id: addButton
        image: "../icons/addIcon.png"
        width: 60; height: 30
        x: 20; y: 40

        onClicked: {
            addDialog.opacity = 1;
        }
    }

    Button {
        id: deleteButton
        image: "../icons/deleteIcon.png"
        width: 60; height: 30
        x: 80; y: 40

        onClicked: {
            deleteDialog.opacity = 1;
        }
    }

    Button {
        id: searchButton
        image: "../icons/searchIcon.png"
        width: 60; height: 30
        x: 140; y: 40

        onClicked: {
            searchDialog.opacity = 1;
        }
    }

    Button {
        id: nextButton
        image: "../icons/nextIcon.png"
        width: 40; height: 30
        x: 130; y: 80

        onClicked: { 
            if (curr < size) { 
                curr++;
                refreshNotes();
            }
        }
    }

    Button {
        id: prevButton
        image: "../icons/prevIcon.png"
        width: 40; height: 30
        x: 50; y: 80

        onClicked: {
            if (curr > 1) {
                curr--;
                refreshNotes();
            }
        }
    }

    DateTimeFormatter {
        id: myDateTime
        dateTime: "2000-01-01 00:00:00"
        dateTimeFormat: "yyyy-MM-dd hh:mm"
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
                    myDateTime.dateTime = note[1] + " " + note[2] + ":00";
                    notesObject.addNote(note[0], myDateTime.dateTime);
                }
            } else {
                    myDateTime.dateTime = currentDateTime() + ":00";
                    notesObject.addNote(note[0], myDateTime.dateTime);
            }

            refreshNotes();
        }
    }

    InputDialog {
        id: searchDialog
        text: "Find a note:"
        size: 100

        onConfirmed: {
            search = input;
            curr = 1; 
            refreshNotes()
        }
    }

    Connections {
        target: notesObject
        
        onSoundAlarm: {
            alarmDialog.text = "ALERT SOUNDED!!!" + "\n\n" + 
                               formatDateTime(alarm) + "\n\n" + notesObject.alarmMessage;
            alarmDialog.opacity = 1;
        }
    }

    Dialog {
        id: deleteDialog
        text: "Confirm removing this note item?"

        onConfirmed: {
            var list = notesObject.noteSet;
            notesObject.removeNote(list[curr-1].index);

            if (curr > 1) { curr--; }

            refreshNotes();
        }
    }

    Dialog {
        id: alarmDialog
        text: "ALERT SOUNDED!!!"
        alert: true
    }

    Script {
        function refreshNotes()
        {
            notesObject.setSearch(search);
            var list = notesObject.noteSet;
            size = list.length;
           
            if (size < 1) curr = 0;
            else if (size > 0 && curr == 0) curr = 1;
          
            if (size > 0) {
                noteLabel.text = list[curr-1].message;
                datetimeLabel.text = formatDateTime(list[curr-1].alarm);
            } else {
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
    }

    Service {
        id: notesService
        interfaceName: "com.nokia.qt.examples.NotesManager"
        serviceName: "NotesManagerService"
        version: "1.2"
    }

    Component.onCompleted: {
        var list = notesObject.noteSet;
        if (list.length > 0) {curr = 1;}
        
        refreshNotes();
    }
}
