import Qt 4.7
import com.nokia.mobility 1.0
import "contents" as TimeScape

Rectangle {
    id: topItem
    width: 360
    height: 640
    x: 0
    y: 0

    color: "#080808";
    state: "MonthView";

    SystemPalette { id: activePalette }

//    Organizer {
//       id : organizer
//    }

    // Quick hack top menu bar to change views
    states: [
        State {name: "MonthView"; PropertyChanges { target: monthView; opacity: 1; }},
        State {name: "TimelineView"; PropertyChanges { target: timelineView; opacity: 1; }},
        State {name: "DetailsView"; PropertyChanges { target: detailsView; opacity: 1; }},
        State {name: "SettingsView"; PropertyChanges { target: settingsView; opacity: 1; }}
    ]
    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity"
                easing.type: "OutBounce"
                duration: 100
            }
        }
    ]
    Rectangle {
        id: menuBar;
        y:0;
        height:childrenRect.height;
        width: parent.width;
        gradient: Gradient {
            GradientStop { position: 0.0; color: activePalette.dark }
            GradientStop { position: 1.0; color: Qt.darker(activePalette.dark); }
        }
        Row {
            spacing: 2
            anchors.horizontalCenter: parent.horizontalCenter;
            TimeScape.Button {text: "Month"; onClicked: topItem.state="MonthView";}
            TimeScape.Button {text: "Timeline";onClicked: topItem.state="TimelineView";}
            TimeScape.Button {text: "Settings";onClicked: topItem.state="SettingsView";}
            TimeScape.Button {text: "Details";onClicked: topItem.state="DetailsView";}
        }
    }

    Item {
        id: contentArea;
        anchors.top: menuBar.bottom;
        anchors.left: topItem.left;
        anchors.right: topItem.right;
        anchors.bottom: topItem.bottom;

        // TODO these should be components too
        Rectangle {
            id: monthView;
            color: "#808000";
            width: 360;
            height: 600;
            opacity: 0;
            anchors.fill: contentArea;

            Loader {id: monthLoader; anchors.fill: parent; source: "contents/monthview.qml";}
        }
        Rectangle {
            id: timelineView;
            color: "#008080";
            width: 360;
            height: 600;
            opacity: 0;
            anchors.fill: contentArea;

            Loader {id: timelineLoader; anchors.fill: parent; source: "contents/timelineview.qml";}
        }
        Rectangle {
            id: detailsView;
            color: "#008000";
            width: 360;
            height: 600;
            opacity: 0;
            anchors.fill: contentArea;

            Loader {id: detailLoader; anchors.fill: parent; source: "contents/detailsview.qml";}
        }
        Rectangle {
            id: settingsView;
            color: "#F08000";
            width: 360;
            height: 600;
            opacity: 0;
            anchors.fill: contentArea;

            Loader {id: settingsLoader; anchors.fill: parent; source: "contents/settingsview.qml";}
        }
    }
}
