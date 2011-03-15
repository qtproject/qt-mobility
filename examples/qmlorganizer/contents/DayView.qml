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
    id:dayView
    property variant itemIds:calendar.organizer.itemIds(calendar.currentDate, new Date(calendar.year, calendar.month, calendar.day+1))

    anchors.fill: parent


    Repeater {
        model : ListModel {
                ListElement {hour : "0:00"}
                ListElement {hour : "1:00"}
                ListElement {hour : "2:00"}
                ListElement {hour : "3:00"}
                ListElement {hour : "4:00"}
                ListElement {hour : "5:00"}
                ListElement {hour : "6:00"}
                ListElement {hour : "7:00"}
                ListElement {hour : "8:00"}
                ListElement {hour : "9:00"}
                ListElement {hour : "10:00"}
                ListElement {hour : "11:00"}
                ListElement {hour : "12:00"}
                ListElement {hour : "13:00"}
                ListElement {hour : "14:00"}
                ListElement {hour : "15:00"}
                ListElement {hour : "16:00"}
                ListElement {hour : "17:00"}
                ListElement {hour : "18:00"}
                ListElement {hour : "19:00"}
                ListElement {hour : "20:00"}
                ListElement {hour : "21:00"}
                ListElement {hour : "22:00"}
                ListElement {hour : "23:00"}
            }

        Rectangle {
            width : dayView.width
            height : dayView.height / 24
            y:index * height
            Column {
                Rectangle {
                    height : 1
                    width : dayView.width
                    color : "lightsteelblue"
                }
                Text {
                    text: hour
                    color : "steelblue"
                    font.pointSize: 12
                }
            }
        }
    }

    Repeater {
        model:itemIds
        ItemView {
            property int timePos: (startTime/(24*60))*dayView.height
            focus: true
            width:dayView.width - 50
            height: endTime< startTime ? 30: dayView.height * (endTime - startTime)/(24*60)
            x: dayView.x + 50
            y: dayView.y + timePos


            opacity : dayView.opacity * 0.8
            itemId: modelData

        }
    }
}
