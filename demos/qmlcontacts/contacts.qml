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
import QtMobility.contacts 1.1
import "contents"
Item {
    id: screen; width: 360; height: 640
    property string viewType : "contactListView"

    Rectangle {
        id: background
        anchors.fill: parent;

        color: "#343434";
        Image { source: "contents/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 1 }

        ContactModel {
            id: contactModel
            Component.onCompleted : {
                if (manager == "memory")
                    contactModel.importContacts("contents/example.vcf");
            }
        }
        TitleBar { id: titleBar; width: parent.width; height: 40; opacity: 0.9
                   managerName:contactModel.manager
                   contactsCount:mainList.count
                   statusString:contactModel.error
        }

        ToolBar {
            id: toolBar
            height: 40; anchors.bottom: parent.bottom; width: parent.width; opacity: 0.9
            button1Label: "Change manager"
            button2Label: "New contact"
            onButton1Clicked: {
                messageBox.messageString = "Not implemented :("
                if (viewType == "contactListView")
                    viewType = "managerListView";
                else if (viewType == "detailListView") {
                    //Delete contact
                }


            }
            onButton2Clicked: {
                messageBox.messageString = "Not implemented :("
                if (viewType = "contactListView") {
                    //add new contact
                } else if (viewType == "detailListView") {
                    //add new detail
                }
            }
        }
        Component {
            id: listdelegate

            Rectangle {
                id: wrapper
                border.width: 2
                height: 36;
                width: mainList.width;

                property color topColor: "#999999";
                property color bottomColor: "#444444";
                property real detailsOpacity: 1
                property int littleDetailsMode: 0;
                property int bigDetailsMode: 0;

                gradient: Gradient {
                     GradientStop { position: 0.0; color: topColor }
                     GradientStop { position: 1.0; color: bottomColor }
                }

                MouseArea {
                    id: mr
                    width: screen.width;
                    height: wrapper.height;
                    anchors.centerIn: parent;
                    onClicked: { littleDetailsMode = !littleDetailsMode; mainList.currentIndex = index; }
                }
                Column {
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
                                anchors.margins: 2;

                                Image {
                                    id: avatar
                                    anchors.fill: parent;
                                    anchors.margins: 2;

                                    source: model.avatar;
                                    fillMode: Image.PreserveAspectFit
                                    smooth:true
                                }
                                Image {
                                    anchors.fill: parent;
                                    anchors.margins: 2;
                                    source: "contents/images/default.svg";
                                    opacity: avatar.status == Image.Ready ? 0 : 1;
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
                                    text: model.display
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
                            id: buttonBox
                            x: wrapper.width - 6 - childrenRect.width;
                            y: 4;
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
                                Item {
                                    height:childrenRect.height
                                    width: childrenRect.width
                                    MediaButton {
                                        id: viewButton
                                        text: "More..."
                                        opacity: 0;
                                        onClicked: wrapper.bigDetailsMode = 1;
                                    }
                                    MediaButton {
                                        id: smallAgainButton
                                        text: "Back";
                                        anchors.top:viewButton.top;
                                        opacity: 0;
                                        onClicked: wrapper.bigDetailsMode = 0;
                                    }
                                }
                            }
                        }
                    }
                    ListView {
                        id: detailView
                        snapMode:ListView.SnapToItem
                        width: wrapper.width;
                        focus: true
                        opacity: 0
                        keyNavigationWraps: true


                        model:contact.details
                        delegate: Component {
                            Item {
                                width: detailView.width;
                                height:100;


                                Column {
                                    spacing: 5
                                    Text {
                                       text:  definitionName;
                                       color:"white";
                                       width: detailView.width;
                                       height: 20;
                                    }
                                    ListView {
                                        id:fieldView
                                        snapMode:ListView.SnapToItem
                                        width: detailView.width;
                                        height: 40

                                        model: fieldNames
                                        property ContactDetail detail: modelData

                                        delegate: Item {
                                            height: parent.height
                                            width: parent.width
                                            Row {
                                                Text {
                                                    text: modelData
                                                    width: detailView.width * 0.5;
                                                    height: 20;
                                                }
                                                TextEdit {
                                                   width: detailView.width * 0.5;
                                                   height: 20;
                                                   text:fieldView.detail.value(modelData).toString();
                                                   color:"yellow";
                                                   //onTextChanged: {field.value = text;}
                                                }
                                            } //row
                                        } //delegate
                                    }
                                }
                            }
                        }//delegate
                    }//detail list view
                }
                states: [
                        State {
                            name: "List";
                            when: mainList.currentIndex != index || wrapper.littleDetailsMode == 0
                            PropertyChanges { target: wrapper; detailsOpacity: 0; }
                            PropertyChanges { target: wrapper; topColor: "#333333"; }
                            PropertyChanges { target: wrapper; bottomColor: "#111111"; }
                            PropertyChanges { target: buttonBox; x: wrapper.width + 6; }
                            PropertyChanges { target: detailView; opacity:0 }
                        },
                        State {
                            name: "MiniDetails"
                            when: (mainList.currentIndex == index) && (wrapper.littleDetailsMode == 1) && (wrapper.bigDetailsMode == 0);
                            PropertyChanges { target: viewButton; opacity: 1; }
                            PropertyChanges { target: smallAgainButton; opacity: 0; }
                            PropertyChanges { target: wrapper; height: Math.max(mainLabel.height + details.height + 4, buttonBox.height + 8); }
                            PropertyChanges { target: mainList; explicit: true; contentY: wrapper.y } // XXX I don't think this should be here
                            PropertyChanges { target: detailView; opacity:0 }
                        },
                        State {
                            name: "Details"
                            when: (mainList.currentIndex == index) && (wrapper.bigDetailsMode == 1);
                            PropertyChanges { target: wrapper; height: mainList.height; }
                            PropertyChanges { target: viewButton; opacity: 0; }
                            PropertyChanges { target: smallAgainButton; opacity: 1; }
                            PropertyChanges { target: mainAvatar; height: 96; }
                            PropertyChanges { target: mainList; explicit: true; contentY: wrapper.y }
                            PropertyChanges { target: mainList; interactive: false; }
                            PropertyChanges { target: detailView; opacity:1;}
                            PropertyChanges { target: detailView; height:wrapper.height - 100;}
                        }
                ]

                transitions:  [
                    Transition {
                        from: "List"
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
                        from: "MiniDetails"
                        to: "Details"
                        reversible: false
                        ParallelAnimation {
                            NumberAnimation { duration: 250; properties: "contentY,opacity"; }
                            SequentialAnimation {
                                NumberAnimation { duration: 100; properties: "detailsOpacity,height" }
                                ParallelAnimation {
                                    ColorAnimation { duration: 100; properties: "topColor, bottomColor";}
                                    NumberAnimation { duration: 150; properties: "x"; }
                                }
                            }
                        }
                    },
                    Transition {
                        from: "Details"
                        to: "MiniDetails"
                        reversible: false
                        ParallelAnimation {
                            NumberAnimation { duration: 250; properties: "contentY,opacity"; }
                            SequentialAnimation {
                                ParallelAnimation {
                                    NumberAnimation { duration: 150; properties: "x"; }
                                    ColorAnimation { duration: 200; properties: "topColor, bottomColor";}
                                }
                                NumberAnimation { duration: 200; properties: "detailsOpacity,height" }
                            }
                        }
                    },
                    Transition {
                        from: "MiniDetails"
                        to: "List"
                        reversible: false
                        SequentialAnimation {
                            NumberAnimation { duration: 100; properties: "x"; }
                            ParallelAnimation{
                                NumberAnimation { duration: 150; properties: "detailsOpacity,height" }
                                ColorAnimation { duration: 150; properties: "topColor, bottomColor";}
                            }
                        }
                    }
                ]
            }
        }
        Item {
            id: views
            width: parent.width
            anchors.top: titleBar.bottom; anchors.bottom: toolBar.top
            ListView {
                id: mainList
                model: contactModel
                width: views.width; height: views.height
                delegate: listdelegate
                highlightFollowsCurrentItem: false
                focus: true
                anchors.fill: parent
                keyNavigationWraps: true
                opacity: 0.8
            }
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
            anchors.right: views.right
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

        Rectangle {
            id:messageBox
            property string messageString: ""
            opacity:0
            height:50
            width:parent.width
            anchors.top:parent.top
            anchors.topMargin:200
            border.width: 0
            radius: 10

            color:"white"
            Text {
                text:messageBox.messageString
                color:"red"
                font.pointSize:20
                horizontalAlignment:Text.AlignHCenter
            }

            // Only show the messageBox when messageString is not empty.
            states: [
                State {
                    name: "ShowMessage"; when: messageBox.messageString != ""
                    PropertyChanges { target: messageBox; opacity: 1 }
                    PropertyChanges { target: timer; running: true }
                }
            ]
            transitions: [
                Transition {
                    NumberAnimation { property: "opacity"; duration: 500; to:0 }
                }
            ]
            Timer {
                id:timer
                interval: 1000; running: false; repeat: false
                onTriggered: {
                    messageBox.messageString = ""
                    running = false
                }
            }

        }

    //    ListView {
    //        id:managerList
    //        model:contactModel.availableManagers
    //        delegate: {

    //        }
    //        width: parent.width; height: parent.height
    //        highlightFollowsCurrentItem: true
    //        focus: true
    //        anchors.fill: parent
    //        keyNavigationWraps: true
    //    }
    }
}
// ![0]
