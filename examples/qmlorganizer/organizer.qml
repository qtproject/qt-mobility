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
import "contents"

Rectangle {
         id: calendar
         width: 400
         height: 640
         property date currentDate:new Date()
         property int year:currentDate.getFullYear()
         property int month:currentDate.getMonth()
         property int day:currentDate.getDate()
         property int weekDay:currentDate.getDay()
         property string status:currentDate.toDateString()

        color: "#343434";
        Image { source: "contents/images/stripes.png"; fillMode: Image.Tile; anchors.fill: parent; opacity: 1 }

        state: "MonthView";

        SystemPalette { id: activePalette }
        property OrganizerModel organizer:OrganizerModel{
            id: organizer
            manager:"qtorganizer:memory:id=qml"
            startPeriod:'2009-01-01'
            endPeriod:'2012-12-31'
            autoUpdate:true
            Component.onCompleted : {
                if (managerName == "memory")
                    organizer.importItems(Qt.resolvedUrl("contents/test.ics"));
            }
        }



        MenuBar { id: menuBar; width: parent.width; height: 35; opacity: 0.9; info: organizer.error + "\nTotal:" + organizer.itemCount }
        StatusBar {
            id: statusBar; status:calendar.status; width: parent.width; height: 35; opacity: 0.9; anchors.bottom: calendar.bottom
            onLeftClicked: {
                if (calendar.state == "MonthView") {
                    calendar.currentDate = new Date(calendar.year, calendar.month - 1, calendar.day);
                } else if (calendar.state == "WeekView") {
                    calendar.currentDate = new Date(calendar.year, calendar.month , calendar.day - 7);
                } else if (calendar.state == "DayView" || calendar.state == "TimelineView") {
                    calendar.currentDate = new Date(calendar.year, calendar.month , calendar.day - 1);
                }

            }
            onRightClicked: {
                if (calendar.state == "MonthView") {
                    calendar.currentDate = new Date(calendar.year, calendar.month + 1, calendar.day);
                } else if (calendar.state == "WeekView") {
                    calendar.currentDate = new Date(calendar.year, calendar.month , calendar.day + 7);
                } else if (calendar.state == "DayView" || calendar.state == "TimelineView") {
                    calendar.currentDate = new Date(calendar.year, calendar.month , calendar.day + 1);
                }
            } //rightClick
        }

        states: [
            State {name: "MonthView"; PropertyChanges { target: monthView; opacity: 1; }},
            State {name: "TimelineView"; PropertyChanges { target: timelineView; opacity: 1; }},
            State {name: "WeekView"; PropertyChanges { target: weekView; opacity: 1; }},
            State {name: "DayView"; PropertyChanges { target: dayView; opacity: 1; }},
            State {name: "AgenderView"; PropertyChanges { target: agenderView; opacity: 1; }},
            State {name: "DetailsView"; PropertyChanges { target: detailsView; opacity: 1; }}
        ]
        transitions: [
            Transition {
                NumberAnimation {
                    properties: "opacity"
                    easing.type: "Linear"
                    duration: 10
                }
            }
        ]


        Item {
            id: contentArea;
            anchors.top: menuBar.bottom;
            anchors.left: calendar.left;
            anchors.right: calendar.right;
            anchors.bottom: statusBar.top;

            MonthView {
                id: monthView;
                width: calendar.width;
                height: calendar.height - menuBar.height - statusBar.height;
                opacity: 0;
                month:calendar.month
                year:calendar.year
                anchors.fill: contentArea;
            }
            TimelineView {
                id: timelineView;
                width: calendar.width;
                height: calendar.height - menuBar.height - statusBar.height;
                opacity: 0;
                anchors.fill: contentArea;
            }
            WeekView {
                id: weekView;
                width: calendar.width;
                height: calendar.height - menuBar.height - statusBar.height;
                opacity: 0;
                anchors.fill: contentArea;
            }
            DayView {
                id: dayView;
                width: calendar.width;
                height: calendar.height - menuBar.height - statusBar.height;
                opacity: 0;
                anchors.fill: contentArea;
            }

            AgenderView {
                id: agenderView;
                width: calendar.width;
                height: calendar.height - menuBar.height - statusBar.height;
                opacity: 0;
                anchors.fill: contentArea;
            }
            DetailsView {
                id: detailsView;
                width: calendar.width;
                height: calendar.height - menuBar.height - statusBar.height;
                opacity: 0;
                anchors.fill: contentArea;
            }

        }
}
