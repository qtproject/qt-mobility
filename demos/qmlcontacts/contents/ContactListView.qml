/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import QtMobility.contacts 1.1

Item {
    id: mainItem
    property variant contacts
    property alias visibleArea: mainList.visibleArea
    property alias count: mainList.count

    signal newContact
    signal openContact(variant contact)

    ListView {
        id: mainList
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: toolBar.top
        highlightFollowsCurrentItem: false
        focus: true
        keyNavigationWraps: true
        opacity: 0.8
        clip: true

        model: contacts

        delegate:
            Rectangle {
                id: wrapper
                border.width: 2
                height: 36;
                width: mainList.width;

                property color topColor: "#999999";
                property color bottomColor: "#444444";
                property real detailsOpacity: 1
                property int littleDetailsMode: 0;

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
                                    id: thumbnail
                                    anchors.fill: parent;
                                    anchors.margins: 2;

                                    source: model.contact.thumbnail;
                                    fillMode: Image.PreserveAspectFit
                                    smooth:true
                                }
                                Image {
                                    anchors.fill: parent;
                                    anchors.margins: 2;
                                    source: "images/default.svg";
                                    opacity: thumbnail.status == Image.Ready ? 0 : 1;
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
                                    //text: model.contact.displayLabel
                                    text: model.contact.name.firstName
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
                                        text: model.contact.displayLabel
                                        color: details.textColor;
                                    }
                                    Text {
                                        text: "State:" + (model.contact.presence.state == Presence.Unknown ? "Unknown" : model.contact.presence.stateText)
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

                                    onClicked: Qt.openUrlExternally("tel:" + model.contact.phoneNumber.number)
                                }
                                MediaButton {
                                    id: textButton
                                    text: "Send Text";

                                    onClicked: Qt.openUrlExternally("sms:" + model.contact.phoneNumber.number)
                                }
                                Item {
                                    height:childrenRect.height
                                    width: childrenRect.width
                                    MediaButton {
                                        id: viewButton
                                        text: "More..."
                                        opacity: 0;
                                        onClicked: { openContact(model.contact); }
                                    }
                                }
                            }
                        }
                    }
                }
                states: [
                        State {
                            name: "List";
                            when: mainList.currentIndex != index || wrapper.littleDetailsMode == 0
                            PropertyChanges { target: wrapper; detailsOpacity: 0; }
                            PropertyChanges { target: wrapper; topColor: "#333333"; }
                            PropertyChanges { target: wrapper; bottomColor: "#111111"; }
                            PropertyChanges { target: buttonBox; x: wrapper.width + 6; }
                        },
                        State {
                            name: "MiniDetails"
                            when: (mainList.currentIndex == index) && (wrapper.littleDetailsMode == 1);
                            PropertyChanges { target: viewButton; opacity: 1; }
                            PropertyChanges { target: wrapper; height: Math.max(mainLabel.height + details.height + 4, buttonBox.height + 8); }
                            PropertyChanges { target: mainList; explicit: true; contentY: wrapper.y } // XXX I don't think this should be here
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
    ToolBar {
        id: toolBar
        height: 40
        anchors.bottom: parent.bottom
        width: parent.width
        opacity: 0.9
        labels: ["Change manager", "New contact"]
        onButtonClicked: {
            if (index == 1) {
                newContact();
            } else {
                messageBox.messageString = "Not implemented :("
            }
        }
    }
}
