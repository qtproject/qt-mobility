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
import "month.js" as Month
import QtMobility.organizer 1.1

Rectangle  {
    id:monthView
    property int month
    property int year
    property date startDay:new Date(year, month, 1)
    property int startWeekday:startDay.getDay()
    anchors.fill: parent

    Grid {
        id:container
        anchors.fill: parent
        columns: 7
        Repeater {
            model:["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
            Rectangle { width: container.width / 7
                        height: 35
                        color: "lightgray"
                        border.color: "#3f4947"
                        Text { text: modelData
                            font.bold: true
                            verticalAlignment: Text.AlignVCenter
                            style: Text.Sunken
                            styleColor: "#1365f3"
                            font.pointSize: 14
                            anchors.centerIn: parent
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                calendar.state = "WeekView";
                            }
                        }
            }
        }

        Repeater { model: 42
                   Rectangle {
                       id:dayContainer
                       width: container.width / 7
                       height: (container.height - 35) / 6
                       color:  {
                                  if (Month.isToday(startDay,   index - startWeekday +1))
                                      return "lightsteelblue";
                                  else if (calendar.organizer.containsItems(Month.dateOfThisDay(startDay,   index - startWeekday +1), Month.dateOfThisDay(startDay,   index - startWeekday +2)))
                                      return "yellow";
                                  else
                                      return Month.getColorOfDay(startDay,   index - startWeekday +1);
                              }
                       border.color: "black"
                       Text {
                           color: "#6ba24b";
                           text: Month.getDayOfMonth(startDay,   index - startWeekday +1)
                           font.bold: true
                           style: Text.Raised
                           font.pointSize: 12
                           anchors.centerIn: parent
                       }

                       MouseArea {
                           hoverEnabled:true
                           anchors.fill: parent
                           onEntered: {
                               dayContainer.border.color = "#1365f3";
                               dayContainer.border.width = 3;
                               dayContainer.radius = 5
                           }
                           onExited: {
                               dayContainer.border.color = "black";
                               dayContainer.border.width = 1;
                               dayContainer.radius = 0;
                           }
                           onClicked: {
                               calendar.currentDate = new Date(calendar.year, calendar.month, index - startWeekday +1);
                               calendar.state = "DayView";
                           }
                       }
                   }
        }
    }
}
