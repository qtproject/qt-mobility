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
            blah.contacts();
        }
        function gotContacts(c) {
            if(c == undefined){
                return;
            }
/*
            print("Got contacts: " + c.name);
            print(" Available actions: " + c.availableActions);
            print(" details: " + c.details);detailsOpacity
*/
            var o = c.values("OnlineAccount");
            var q = c.values("Presence");


            nameModel.append({"name": c.name, "accountPath": "Account: " + o.AccountPath, "presence": "Status: " + q.Presence, "email": c.email, "avatarSource": "qrc:/default.svg"});

/*
            var j;
            for(j in c.details){                
                var o = c.values(c.details[j]);
                var i;
                for(i in o){
                    print(" "+ c.details[j] + "/" + i + ": " + o[i]);
                }
            }
*/

        }
        function clickedList(index) {
            mainList.currentIndex = index;
        }
    }
    Component.onCompleted: startup();

    QMLContactManagerAsync {
        id: "blah"

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
            height: 30;
//            width: parent.width;

            property color topColor: "#333333";
            property color bottomColor: "#111111";
            property real detailsOpacity: 0

            gradient: Gradient {
                 GradientStop { position: 0.0; color: topColor }
                 GradientStop { position: 1.0; color: bottomColor }
             }

            Item {
                id: mainAvatar;
                anchors.left: parent.left;
                anchors.top: parent.top;
                width: avatarFrame.width;
                height: avatarFrame.height;
                anchors.leftMargin:4;

                Rectangle {
                    id: avatarFrame;
                    border.width: 2;
                    radius: 4;
                    height: wrapper.height-6
                    width: height;
                    x: 3; y: 3;
                }

                Image {
                    id: avatar
                    anchors.fill: avatarFrame;
                    anchors.leftMargin: 3;
                    anchors.rightMargin: 3;
                    anchors.topMargin: 3;
                    anchors.bottomMargin: 3;

                    source: avatarSource
                    fillMode: Image.PreserveAspectFit
                }
            }

            Item {
                id: mainLabel;
                height: nameTxt.height + 16;
                property real contactId: 0;
                anchors.left: mainAvatar.right;
                anchors.right: parent.right;
                anchors.leftMargin:8;
                anchors.rightMargin: 4;
                anchors.topMargin: 4;
                anchors.bottomMargin: 4;

                Text {
                    x:8; y:8;
                    anchors.left: parent.left;
                    anchors.right: parent.right;
                    id: nameTxt
                    text: name
                    color: "white";
                }
            }

            Item {
                id: details
                property color textColor: "#ffffdd";
                anchors.top: mainLabel.bottom;
                anchors.bottom: parent.bottom;
                anchors.left: mainAvatar.right;
                anchors.right: parent.right;
                anchors.leftMargin:8;
                anchors.rightMargin: 4;
                anchors.bottomMargin: 4;
                opacity: wrapper.detailsOpacity

                Column {
                    Text {
                        id: emailId
                        text: email
                        color: details.textColor;
                    }
                    Row {
                        spacing: 5
                        Text {
                            id: accountPathId
                            text: accountPath
                            color: details.textColor;
                        }
                        Text {
                            id: presenceId
                            text: presence
                            color: details.textColor;
                        }
                    }
                }
            }

            states: State {
                name: "Details"
                PropertyChanges { target: wrapper; detailsOpacity: 1; }
                PropertyChanges { target: wrapper; topColor: "#666666"; }
                PropertyChanges { target: wrapper; bottomColor: "#222222"; }
                PropertyChanges { target: wrapper; height: mainLabel.height + emailId.height + accountPathId.height + presenceId.height; }
            }

            transitions: Transition {
                from: ""
                to: "Details"
                reversible: true
                ParallelAnimation {
                    ColorAnimation { duration: 150; properties: "topColor, bottomColor";}
                    NumberAnimation { duration: 100; properties: "detailsOpacity,height" }
                }
            }
            MouseArea {
                id: mr
                width: topItem.width;
                height: wrapper.height;
                anchors.centerIn: parent;
                onClicked: wrapper.state == "" ? wrapper.state = "Details" :  wrapper.state = "";
            }
        }
    }

    Component {
        id: listhighlight
        Rectangle {
            width: parent.width-8
            height: 40
            color: "lightsteelblue"
            radius: 5
        }
    }

    ListView {
        id: mainList
        model: nameModel
        width: parent.width; height: parent.height
        delegate: listdelegate
        highlight: listhighlight
        //highlightFollowsCurrentItem: true
        focus: true
        anchors.fill: parent
        highlightMoveSpeed: 5000
    }


    ListModel {
        id: nameModel
    }

    // Attach scrollbar to the right edge of the view.
    ScrollBar {
        id: verticalScrollBar
        opacity: 0.1
        orientation: "Vertical"
        position: mainList.visibleArea.yPosition
        pageSize: mainList.visibleArea.heightRatio
        width: 20
        height: mainList.height
        anchors.right: mainList.right
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
