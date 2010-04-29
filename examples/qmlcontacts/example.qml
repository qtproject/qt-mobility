import QMLContactManagerAsync 1.0
import QmlContact 1.0
import Qt 4.6

Rectangle {
    id: topItem
    width: 320
    height: 480
    x: 0
    y: 0

    color: "#080808";

    Script {
        function startup() {
            manager.contacts();
        }
        function gotContacts(c) {
            if(c == undefined){
                return;
            }

            var q = c.values("Presence");

            nameModel.append({"name": c.name,
                                "presence": "Status: " + q.Presence,
                                "email": c.email,
                                "avatarSource": c.avatar ? c.avatar : "qrc:/default.svg",
                                "hasThumbnail" : c.hasThumbnail,
                                "avatarImage": c.thumbnail,
                                "interestLabel" : c.interestLabel,
                                "interest" : c.interest});

        }
    }

    Component.onCompleted: startup();

    QMLContactManagerAsync {
        id: "manager"

        manager: "memory"
        onDataChanged: print("Data changed!");
        onContactsAdded: print("Contacts added: " + contactIds);
        onContactsLoaded: gotContacts(contact);
    }

    Component {
        id: listdelegate        
        Rectangle {
            id: wrapper            
            border.width: 2
            height: mainLabel.height;
            width: mainList.width;

            property color topColor: "#333333";
            property color bottomColor: "#111111";
            property real detailsOpacity: 0

            gradient: Gradient {
                 GradientStop { position: 0.0; color: topColor }
                 GradientStop { position: 1.0; color: bottomColor }
            }

            Row {
                spacing: 2
                Item {
                    id: mainAvatar;
                    height: wrapper.height;
                    width: height;

                    Rectangle {
                        border.width: 2;
                        radius: 4;
                        anchors.fill: parent;
                        anchors.margins: 3;

                        Image {
                            id: avatar
                            anchors.fill: parent;
                            anchors.margins: 3;

                            pixmap: avatarImage
                            source: hasThumbnail ? "" : avatarSource;
                            fillMode: Image.PreserveAspectFit
                        }
                    }
                }

                Column {
                    Item {
                        id: mainLabel;
                        width: nameTxt.width
                        height: nameTxt.height + 16;
                        Text {
                            id: nameTxt
                            y: 8;
                            text: name
                            color: "white";
                        }
                    }

                    Item {
                        id: details
                        property color textColor: "#ffffdd";
                        opacity: wrapper.detailsOpacity
                        height: childrenRect.height + 6;
                        width: childrenRect.width;
                        Column {
                            Text {
                                text: interestLabel + interest
                                color: details.textColor;
                            }
                            Text {
                                text: presence
                                color: details.textColor;
                            }
                        }
                    }
                }
            }

            states: State {
                name: "Details"
                when: wrapper.ListView.isCurrentItem;
                PropertyChanges { target: wrapper; detailsOpacity: 1; }
                PropertyChanges { target: wrapper; topColor: "#999999"; }
                PropertyChanges { target: wrapper; bottomColor: "#444444"; }
                PropertyChanges { target: wrapper; height: mainLabel.height + details.height + 4; }
            }

            transitions:  [
                Transition {
                    from: ""
                    to: "Details"
                    reversible: false
                    SequentialAnimation {
                        NumberAnimation { duration: 100; properties: "detailsOpacity,height" }
                        ColorAnimation { duration: 100; properties: "topColor, bottomColor";}
                    }
                },
                Transition {
                    to: ""
                    from: "Details"
                    reversible: false
                    SequentialAnimation {
                        NumberAnimation { duration: 100; properties: "detailsOpacity,height" }
                        ColorAnimation { duration: 100; properties: "topColor, bottomColor";}
                    }
                }
            ]
            MouseArea {
                id: mr
                width: topItem.width;
                height: wrapper.height;
                anchors.centerIn: parent;
                onClicked: mainList.currentIndex = index;
            }
        }
    }

    ListView {
        id: mainList
        model: nameModel
        width: parent.width; height: parent.height
        delegate: listdelegate
        highlightFollowsCurrentItem: false
        focus: true
        anchors.fill: parent
        highlightMoveSpeed: 5000
        keyNavigationWraps: true
    }

    ListModel {
        id: nameModel
    }

    // Attach scrollbar to the right edge of the view.
    ScrollBar {
        id: verticalScrollBar
        opacity: 0
        orientation: "Vertical"
        position: mainList.visibleArea.yPosition
        pageSize: mainList.visibleArea.heightRatio
        width: 20
        height: mainList.height
        anchors.right: mainList.right
        fgColor: "white"
        // Only show the scrollbar when the view is moving.
        states: [
            State {
                name: "ShowBars"; when: mainList.moving
                PropertyChanges { target: verticalScrollBar; opacity: 1 }
            }
        ]
        transitions: [ Transition { NumberAnimation { property: "opacity"; duration: 400 } } ]
    }
}

// ![0]
