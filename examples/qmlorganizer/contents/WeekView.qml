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

Rectangle
{
    id:weekView
    anchors.fill: parent
    ListView {
        id : dayList
        anchors.fill: parent
        clip: true
        focus: true
        opacity : parent.opacity
        preferredHighlightBegin: dayList.height * 0.5
        preferredHighlightEnd: preferredHighlightBegin
        highlightFollowsCurrentItem : true
        highlightMoveSpeed : 2000
        keyNavigationWraps : true
        Component.onCompleted : positionViewAtIndex(currentIndex, ListView.Beginning)
        onOpacityChanged: {
                currentIndex = calendar.weekDay;
         }

        model : ListModel {
                ListElement {day : "Sunday"}
                ListElement {day : "Monday"}
                ListElement {day : "Tuesday"}
                ListElement {day : "Wednesday"}
                ListElement {day : "Thursday"}
                ListElement {day : "Friday"}
                ListElement {day : "Saturday"}
         }

        delegate : Rectangle {
                id:weekDayDelegate
                width : dayList.width
                height :  childrenRect.height > dayList.height / 7  ? childrenRect.height  : dayList.height / 7
                focus: true

               FocusScope {
                   focus: true
                    Column {
                        focus: true
                        Rectangle {
                            height : 2
                            width : dayList.width
                            color : "lightsteelblue"
                        }
                        Text {
                            text: day
                        }

                        Repeater {
                            focus: true
                            model:calendar.organizer.itemIds(new Date(calendar.year,
                                                                      calendar.month,
                                                                      index - calendar.weekDay + calendar.day),
                                                             new Date(calendar.year,
                                                                      calendar.month,
                                                                      index - calendar.weekDay + calendar.day + 1))

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
               }
               MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        dayList.currentIndex = index
                        calendar.currentDate = new Date(calendar.year, calendar.month, calendar.day + dayList.currentIndex - calendar.weekDay);
                        calendar.state = "DayView"
                    }
                }
        }

        highlight:  Component {

            Rectangle {
                width: dayList.width
                height: dayList.height /7
                color: "lightsteelblue"
                radius: 5
            }
        }

    }
}

