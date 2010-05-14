import QmlContactModel 1.0
import Qt 4.6
import "contents"

Rectangle {
    id: topItem
    width: 360
    height: 640
    x: 0
    y: 0

    color: "#080808";

    QmlContactModel {
        id: "myModel"
        manager: "memory"
    }

    Component {
        id: listdelegate        
        Rectangle {
            id: wrapper            
            border.width: 2
            height: mainLabel.height + 2;
            width: mainList.width;

            property color topColor: "#333333";
            property color bottomColor: "#111111";
            property real detailsOpacity: 0
            property int detailsMode: 0

            gradient: Gradient {
                 GradientStop { position: 0.0; color: topColor }
                 GradientStop { position: 1.0; color: bottomColor }
            }

            MouseArea {
                id: mr
                width: topItem.width;
                height: wrapper.height;
                anchors.centerIn: parent;
                onClicked: {mainList.currentIndex = index;}
            }

            Row {
                spacing: 2
                Item {
                    id: mainAvatar;
                    height: wrapper.detailsMode == 0 ? wrapper.height : wrapper.height;
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

                            pixmap: model.decoration
                            source: model.avatar;
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
                            text: display
                            color: "white"
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
                                text: model.interestLabel + ": " + model.interest
                                color: details.textColor;
                            }
                            Text {
                                text: model.presenceAvailable ? model.presenceText + " [" + model.presenceMessage + "]" : " ";
                                color: details.textColor;
                            }
                        }
                    }
                }

                Item {
                    id: "buttonBox"
                    // Initially offscreen
                    x: wrapper.width + 6;
                    y: (wrapper.height - childrenRect.height) / 2;
                    height:childrenRect.height
                    opacity: details.opacity;
                    Column {
                        // Buttons
                        MediaButton {
                            id: dialButton;
                            text: "Dial";
                        }
                        MediaButton {
                            id: textButton
                            text: "Send Text";
                        }
                        MediaButton {
                            id: viewButton
                            text: wrapper.detailsMode == 0 ? "View Details" : "Exit";
//                            onClicked: wrapper.detailsMode = 1 - wrapper.detailsMode;
                        }

                    }
                }
            }


            states: [
                    State {
                        name: "MiniDetails"
                        when: wrapper.ListView.isCurrentItem & wrapper.detailsMode == 0;
                        PropertyChanges { target: wrapper; detailsOpacity: 1; }
                        PropertyChanges { target: wrapper; topColor: "#999999"; }
                        PropertyChanges { target: wrapper; bottomColor: "#444444"; }
                        PropertyChanges { target: wrapper; height: Math.max(mainLabel.height + details.height + 4, buttonBox.height + 8); }
                        PropertyChanges { target: buttonBox; x: wrapper.width - 6 - childrenRect.width; }
                    },
                    State {
                        name: "Details"
                        when: wrapper.detailsMode==1;
                        PropertyChanges { target: wrapper.ListView.view; explicit: true; contentY: wrapper.y }
                        PropertyChanges { target: wrapper.ListView.view; interactive: false }
                    }
            ]



            transitions:  [
                Transition {
                    from: ""
                    to: "MiniDetails"
                    reversible: false
                    SequentialAnimation {
                        NumberAnimation { duration: 100; properties: "detailsOpacity,height" }
                        ParallelAnimation {
                            ColorAnimation { duration: 100; properties: "topColor, bottomColor";}
                            NumberAnimation { duration: 150; properties: "x"; }
                        }
                    }
                },
                Transition {
                    to: ""
                    from: "MiniDetails"
                    reversible: false
                    SequentialAnimation {
                        NumberAnimation { duration: 150; properties: "x"; }
                        NumberAnimation { duration: 200; properties: "detailsOpacity,height" }
                        ColorAnimation { duration: 200; properties: "topColor, bottomColor";}
                    }
                }
            ]
        }
    }

    ListView {
        id: mainList
        model: myModel
        width: parent.width; height: parent.height
        delegate: listdelegate
        highlightFollowsCurrentItem: false
        focus: true
        anchors.fill: parent
        highlightMoveSpeed: 5000
        keyNavigationWraps: true
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
