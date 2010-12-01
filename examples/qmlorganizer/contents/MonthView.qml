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
    property int month: topItem.day.getMonth()
    property int year: topItem.day.getFullYear()
    property date startDay:new Date(year, month, 1)
    property int startWeekday:startDay.getDay()


    OrganizerModel {
        id: organizer;
        startPeriod:startDay;
        endPeriod:new Date(year, month+1, 1);
        Component.onCompleted : {
            if (manager == "memory")
                organizer.importItems(Qt.resolvedUrl("2010-FIFA-WorldCup.ics"));
        }
    }

    anchors.fill: parent

    Grid {
        id:container
        anchors.fill: parent
        columns: 7
        Repeater {
            model:["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"]
            Rectangle { width: container.width / 7
                        height: container.height / 7
                        color: "lightgray"
                        border.color: "#3f4947"
                        Text { text: modelData
                               font.pointSize: 10
                               anchors.centerIn: parent
                        }
            }
        }

        Repeater { model: 42
                   Rectangle { width: container.width / 7
                               height: container.height / 7
                               color: Month.getColorOfDay(startDay,   index - startWeekday +1)
                               border.color: "black"
                               Text { text: Month.getDayOfMonth(startDay,   index - startWeekday +1)
                                      font.pointSize: 10
                                      anchors.centerIn: parent
                               }
                   }
        }
    }
}
