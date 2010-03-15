import QMLContactManagerAsync 1.0
import QmlContact 1.0
import Qt 4.6

Rectangle {
    id: topItem
    width: 320
    height: 480
//    border.color: "bg
//    border.width: 5
//    radius: 10
    x: 0
    y: 0

    Script {
        function startup() {
            print("Hello");

            print("Num contacts: " + blah.numContacts);            
            blah.contacts();
        }
        function gotContacts(c) {
            if(c == undefined){
                //print("Error, got null object for gotContacts");
                return;
            }
            print("Got contacts: " + c.name);
            print(" Available actions: " + c.availableActions);
            print(" details: " + c.details);detailsOpacity

            var o = c.values("OnlineAccount");
            var q = c.values("Presence");


            nameModel.append({"name": c.name, "accountPath": "Account: " + o.AccountPath, "presence": "Status: " + q.Presence, "email": c.email});

            var j;
            for(j in c.details){                
                var o = c.values(c.details[j]);
                var i;
                for(i in o){
                    print(" "+ c.details[j] + "/" + i + ": " + o[i]);
                }
            }
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
            radius: 5
            height: 40
            width: topItem.width-2;            
            property real detailsOpacity: 0
            Row {
                Item {
                    property real contactId: 0
                }
                Text {
                    id: nameTxt
                    text: name
                }
                Item {
                    id: details
                    opacity: wrapper.detailsOpacity
                    Text {
                        y: nameTxt.height
                        id: emailId
                        text: email
                    }
                    Text {
                        y: emailId.y + emailId.height
                        id: accountPathId
                        text: accountPath
                    }
                    Text {
                        y: accountPathId.y
                        x: accountPathId.x + accountPathId.width + 5
                        id: presenceId
                        text: presence
                    }
                }                
            }
            /*
            Image {
                id: avatar
                height: wrapper.height-6
                source: avatarsource
                x: wrapper.width - avatar.width - 3
                y: 3                
                opacity: details.opacity
                fillMode: Image.PreserveAspectFit
            }*/
            states: State {
                name: "Details"
                PropertyChanges { target: wrapper; height: presenceId.y + presenceId.height }
                PropertyChanges { target: wrapper; detailsOpacity: 1; }
            }

            transitions: Transition {
                from: ""
                to: "Details"
                reversible: true
                ParallelAnimation {
                    NumberAnimation {
                        duration: 300; property: "detailsOpacity" }
                    NumberAnimation {
                        duration: 300; property: "height"  }
                }
            }
            MouseRegion {
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
        width: 40
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
