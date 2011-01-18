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
    property bool showContact: false

    Rectangle {
        id: background
        anchors.fill: parent;

        color: "#343434";
        Image { source: "contents/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 1 }

        ContactModel {
            id: contactModel
            Component.onCompleted : {
                if (manager == "memory")
                    contactModel.importContacts(Qt.resolvedUrl("contents/example.vcf"));
            }
        }
        TitleBar {
            id: titleBar
            width: parent.width
            height: 40
            opacity: 0.9
            managerName: contactModel.manager
            contactsCount: contactListView.count
            statusString: contactModel.error
        }

        ContactListView {
            id: contactListView
            width: parent.width
            anchors.top: titleBar.bottom
            anchors.bottom: parent.bottom
            x: 0
            contacts: contactModel
            onOpenContact: {
                    screen.showContact = true;
                    contactView.contact = contact;
                    }
            onNewContact: {
                    var contact = Qt.createQmlObject(
                        "import QtMobility.contacts 1.1;" +
                        "Contact {}", contactModel);
                    screen.showContact = true;
                    contactView.contact = contact;
                }
        }

        ContactView {
            id: contactView
            width: parent.width
            anchors.top: titleBar.bottom
            anchors.bottom: parent.bottom
            anchors.left: contactListView.right
            opacity: 0
            onDismissed: {screen.showContact = false;}
            onDeleted: {contactModel.removeContact(id);}
        }

        states: [
                State {
                    name: "List";
                    when: !screen.showContact
                    PropertyChanges { target: contactListView; opacity: 1; }
                    PropertyChanges { target: contactView; opacity: 0; }
                },
                State {
                    name: "Detailed";
                    when: screen.showContact
                    PropertyChanges { target: contactListView; opacity: 0; x: -screen.width }
                    PropertyChanges { target: contactView; opacity: 1; }
                }
            ]
        transitions:  [
                Transition {
                    from: "List"
                    to: "Detailed"
                    reversible: true
                    SequentialAnimation {
                        NumberAnimation { duration: 100; properties: "opacity,x" }
                    }
                }
            ]

        // Attach scrollbar to the right edge of the view.
        ScrollBar {
            id: verticalScrollBar
            opacity: 0
            orientation: "Vertical"
            position: contactListView.visibleArea.yPosition
            pageSize: contactListView.visibleArea.heightRatio
            width: 20
            height: contactListView.height
            anchors.right: contactListView.right
            anchors.top: titleBar.bottom
            fgColor: "white"
            // Only show the scrollbar when the view is moving.
            states: [
                State {
                    name: "ShowBars"; when: contactListView.moving
                    PropertyChanges { target: verticalScrollBar; opacity: 1 }
                }
            ]
            transitions: [ Transition { NumberAnimation { property: "opacity"; duration: 400 } } ]
        }

        // Message box
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
