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
import com.nokia.mobility 1.0
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
                width: topItem.width;
                height: wrapper.height;
                anchors.centerIn: parent;
                onClicked: { littleDetailsMode = !littleDetailsMode; mainList.currentIndex = index; }
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
                        anchors.margins: 2;

                        Image {
                            id: avatar
                            anchors.fill: parent;
                            anchors.margins: 2;

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
                            ListView {
                                //width: parent.width; height: parent.height
                                highlightFollowsCurrentItem: false
                                focus: true
                                //anchors.fill: parent
                                keyNavigationWraps: true

                                model:model.details(model.id)
                                delegate: Component {
                                    //TODO
                                }
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
                        when: (mainList.currentIndex == index) && (wrapper.littleDetailsMode == 1) && (wrapper.bigDetailsMode == 0);
                        PropertyChanges { target: viewButton; opacity: 1; }
                        PropertyChanges { target: smallAgainButton; opacity: 0; }
                        PropertyChanges { target: wrapper; height: Math.max(mainLabel.height + details.height + 4, buttonBox.height + 8); }
                        PropertyChanges { target: mainList; explicit: true; contentY: wrapper.y } // XXX I don't think this should be here
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

    ListView {
        id: mainList
        model: myModel
        width: parent.width; height: parent.height
        delegate: listdelegate
        highlightFollowsCurrentItem: false
        focus: true
        anchors.fill: parent
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
