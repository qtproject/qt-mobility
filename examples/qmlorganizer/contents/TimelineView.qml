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
import "timeline.js" as Timeline

Rectangle {
    id : timelineView
    anchors.fill : parent
    opacity : parent.opacity
    property int year:calendar.year
    property int month:calendar.month
    property int day:calendar.day
    Connections {
        target: calendar
        onCurrentDateChanged : {
            dayList.currentIndex = day - 1;
            monthList.currentIndex = month;
            yearList.currentIndex = year - yearModel.start;
        }
    }

    //Day view
    Rectangle {
        id: dayView
        color: "#9eaf30"
        gradient: Gradient {
            GradientStop {
                position: 0.00;
                color: "#9eaf30";
            }
            GradientStop {
                position: 0.89;
                color: "#ffffff";
            }
        }
        anchors.right : parent.right
        anchors.left : monthView.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        opacity : parent.opacity

        ListView {
            id : dayList
            model : dayModel
            anchors.fill: parent
            clip: true
            focus: true
            opacity : parent.opacity

            delegate : dayDelegate
            highlight: dayHighlight
            preferredHighlightBegin: dayList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightFollowsCurrentItem : true
            highlightMoveSpeed : 2000
            keyNavigationWraps : true

            onCurrentIndexChanged : {
                if (timelineView.opacity > 0) {
                    calendar.currentDate = new Date(timelineView.year, timelineView.month, currentIndex + 1);
                    monthList.currentIndex = timelineView.month;
                    currentIndex = timelineView.day - 1;
                }
            }

        }

        Component {
            id: dayHighlight
            Rectangle {
                width: dayList.width;
                height: dayList.height /7 ;
                color: "lightsteelblue" ;radius: 5
            }
        }

        Component {
            id: dayDelegate
            Item {
                width : dayList.width
                height : childrenRect.height
                Column {
                    Rectangle {
                        height : 1
                        width : dayList.width
                        color : "black"
                    }
                    Text {
                        text: day
                    }
                    Repeater {
                        focus: true
                        model:calendar.organizer.itemIds(new Date(timelineView.year,timelineView.month, index + 1),
                                                                                      new Date(timelineView.year,timelineView.month, index + 2))

                        Text {
                            clip: true
                            focus: true
                            property OrganizerItem oi: calendar.organizer.item(modelData)
                            text: "<a href=\"#\">" + oi.displayLabel + "</a>"
                            onLinkActivated: {
                                    console.log(oi.type + "," + oi.displayLabel + oi.description);
                                   //TODO: goto details view
                            }
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : {
                        dayList.currentIndex = index
                        calendar.state = "DayView"
                    }
                }
            }
        }

        ListModel {
            id : dayModel
            ListElement {day : "1"}
            ListElement {day : "2"}
            ListElement {day : "3"}
            ListElement {day : "4"}
            ListElement {day : "5"}
            ListElement {day : "6"}
            ListElement {day : "7"}
            ListElement {day : "8"}
            ListElement {day : "9"}
            ListElement {day : "10"}
            ListElement {day : "11"}
            ListElement {day : "12"}
            ListElement {day : "13"}
            ListElement {day : "14"}
            ListElement {day : "15"}
            ListElement {day : "16"}
            ListElement {day : "17"}
            ListElement {day : "18"}
            ListElement {day : "19"}
            ListElement {day : "20"}
            ListElement {day : "21"}
            ListElement {day : "22"}
            ListElement {day : "23"}
            ListElement {day : "24"}
            ListElement {day : "25"}
            ListElement {day : "26"}
            ListElement {day : "27"}
            ListElement {day : "28"}
            ListElement {day : "29"}
            ListElement {day : "30"}
            ListElement {day : "31"}
        }
    }



    //Month view
    Rectangle {
        id:monthView
        color : "lightgray"
        width : 30
        anchors.left : yearView.right
        anchors.top : parent.top
        anchors.bottom : parent.bottom

        ListView {
            id : monthList
            model : monthModel
            anchors.fill: parent
            clip: true
            delegate : monthDelegate
            highlight: monthHighlight
            preferredHighlightBegin: monthList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightFollowsCurrentItem : true
            highlightMoveSpeed : 1000
            Component.onCompleted : {
               var now = new Date();
               var month = now.getMonth();
               monthList.currentIndex = month;
               var  d = Date.parse("Feb 31, 2010");
            }
            onCurrentIndexChanged : {
                if (timelineView.opacity > 0) {
                    calendar.currentDate = new Date(timelineView.year, currentIndex, timelineView.day);
                    currentIndex = timelineView.month;
                    dayList.currentIndex = timelineView.day - 1;
                }
            }
        }

        Component {
            id: monthHighlight
            Rectangle {
                width: monthList.width; height: monthList.height / 12 ; color: "lightsteelblue" ;radius: 5
            }
        }

        Component {
            id: monthDelegate
            Item {
                width : monthList.width
                height : monthList.height / 12
                Column {
                    Rectangle {
                        height : 1
                        width : monthList.width
                        color : "black"
                    }
                    Text {
                        text: month
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : monthList.currentIndex = index
                }
            }
        }

        ListModel {
            id : monthModel
            ListElement {month : "Jan"}
            ListElement {month : "Feb"}
            ListElement {month : "Mar"}
            ListElement {month : "Apr"}
            ListElement {month : "May"}
            ListElement {month : "Jun"}
            ListElement {month : "Jul"}
            ListElement {month : "Aug"}
            ListElement {month : "Sep"}
            ListElement {month : "Oct"}
            ListElement {month : "Nov"}
            ListElement {month : "Dec"}
        }
    }

    //Year view
    Rectangle {
        id:yearView
        color : "gray"
        anchors.top : parent.top
        anchors.bottom : parent.bottom
        anchors.left : parent.left
        width : 50


        Component {
            id: yearHighlight
            Rectangle {
                width: yearList.width; height: yearList.height / 10; color: "lightsteelblue" ;radius: 5
            }
        }

        Component {
            id: yearDelegate

            Item {
                width : yearList.width
                height : yearList.height / 10
                Column {
                    Rectangle {
                        height : 1
                        width : yearList.width
                        color : "black"
                    }
                    Text {
                        text: year
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : yearList.currentIndex = index
                }
            }
        }


        ListView {
            id : yearList
            model : yearModel
            delegate : yearDelegate
            anchors.fill: parent
            clip: true
            highlight: yearHighlight

            preferredHighlightBegin: yearList.height * 0.5
            preferredHighlightEnd: preferredHighlightBegin
            highlightRangeMode: "StrictlyEnforceRange"
            highlightFollowsCurrentItem : true

            Component.onCompleted: Timeline.extendYearModel(true);
            onCurrentIndexChanged: {
                Timeline.extendYearModel(false);
                if (timelineView.opacity > 0) {
                    calendar.currentDate = new Date(yearModel.start + currentIndex,  timelineView.month, timelineView.day);
                    monthList.currentIndex = timelineView.month;
                    dayList.currentIndex = timelineView.day - 1;
                }
            }

        ListModel {
            id : yearModel
            property int start : 0;
            property int end : 0;

            ListElement {
                year : 0;
            }
        }
    }
    }
}
