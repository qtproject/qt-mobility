/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Mobility Components.
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
import QtMobility.organizer 1.1
import "contents" as TimeScape
Item {
    id: screen; width: 380; height: 640
    property string viewType : "contactListView"

    Rectangle {
        id: topItem
        anchors.fill: parent;

        color: "#343434";
        Image { source: "contents/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 1 }

        state: "MonthView";

        SystemPalette { id: activePalette }

       // OrganizerModel {id: organizerModelId; manager:'memory'; startPeriod:'2010-08-12T13:22:01'; endPeriod:'2010-09-12T13:22:01'}
        //OrganizerItem {id:organizerItem; guid:'1112232133'}
        // Quick hack top menu bar to change views
        TimeScape.MenuBar { id: menuBar; width: parent.width; height: 40; opacity: 0.9 }

        states: [
            State {name: "MonthView"; PropertyChanges { target: monthView; opacity: 1; }},
            State {name: "TimelineView"; PropertyChanges { target: timelineView; opacity: 1; }},
            State {name: "WeekView"; PropertyChanges { target: weekView; opacity: 1; }},
            State {name: "DayView"; PropertyChanges { target: dayView; opacity: 1; }},
            State {name: "AgenderView"; PropertyChanges { target: agenderView; opacity: 1; }}
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
                Loader {id: monthLoader; opacity:parent.opacity; anchors.fill: parent; source: "contents/monthview.qml";}
            }
            Rectangle {
                id: timelineView;
                color: "#008080";
                width: 360;
                height: 600;
                opacity: 0;
                anchors.fill: contentArea;

                Loader {id: timelineLoader; opacity:parent.opacity; anchors.fill: parent; source: "contents/timelineview.qml";}
            }
            Rectangle {
                id: weekView;
                color: "#008000";
                width: 360;
                height: 600;
                opacity: 0;
                anchors.fill: contentArea;

                Loader {id: weekLoader; opacity:parent.opacity; anchors.fill: parent; source: "contents/weekview.qml";}
            }
            Rectangle {
                id: dayView;
                color: "#F08000";
                width: 360;
                height: 600;
                opacity: 0;
                anchors.fill: contentArea;

                Loader {id: dayLoader; opacity:parent.opacity; anchors.fill: parent; source: "contents/dayview.qml";}
            }
            Rectangle {
                id: agenderView;
                color: "#F08000";
                width: 360;
                height: 600;
                opacity: 0;
                anchors.fill: contentArea;

                Loader {id: agenderLoader; opacity:parent.opacity; anchors.fill: parent; source: "contents/agenderview.qml";}
            }
            Rectangle {
                id: detailsView;
                color: "#F08000";
                width: 360;
                height: 600;
                opacity: 0;
                anchors.fill: contentArea;

                Loader {id: detailsLoader; opacity:parent.opacity; anchors.fill: parent; source: "contents/detailsview.qml";}
            }

        }
}
}
