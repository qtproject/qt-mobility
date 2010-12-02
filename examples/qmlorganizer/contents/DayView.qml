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
    property variant itemIds:calendar.organizer.itemIds(calendar.day)

    anchors.fill: parent

    ListView {
        id : timeList
        anchors.fill: parent
        clip: true
        focus: true
        opacity : parent.opacity
        currentIndex: 16
        preferredHighlightBegin: timeList.height * 0.5
        preferredHighlightEnd: preferredHighlightBegin
        highlightFollowsCurrentItem : true
        highlightMoveSpeed : 2000
        keyNavigationWraps : true

        model : ListModel {
                ListElement {hour : "0:00"}
                ListElement {hour : "0:30"}
                ListElement {hour : "1:00"}
                ListElement {hour : "1:30"}
                ListElement {hour : "2:00"}
                ListElement {hour : "2:30"}
                ListElement {hour : "3:00"}
                ListElement {hour : "3:30"}
                ListElement {hour : "4:00"}
                ListElement {hour : "4:30"}
                ListElement {hour : "5:00"}
                ListElement {hour : "5:30"}
                ListElement {hour : "6:00"}
                ListElement {hour : "6:30"}
                ListElement {hour : "7:00"}
                ListElement {hour : "7:30"}
                ListElement {hour : "8:00"}
                ListElement {hour : "8:30"}
                ListElement {hour : "9:00"}
                ListElement {hour : "9:30"}
                ListElement {hour : "10:00"}
                ListElement {hour : "10:30"}
                ListElement {hour : "11:00"}
                ListElement {hour : "11:30"}
                ListElement {hour : "12:00"}
                ListElement {hour : "12:30"}
                ListElement {hour : "13:00"}
                ListElement {hour : "13:30"}
                ListElement {hour : "14:00"}
                ListElement {hour : "14:30"}
                ListElement {hour : "15:00"}
                ListElement {hour : "15:30"}
                ListElement {hour : "16:00"}
                ListElement {hour : "16:30"}
                ListElement {hour : "17:00"}
                ListElement {hour : "17:30"}
                ListElement {hour : "18:00"}
                ListElement {hour : "18:30"}
                ListElement {hour : "19:00"}
                ListElement {hour : "19:30"}
                ListElement {hour : "20:00"}
                ListElement {hour : "20:30"}
                ListElement {hour : "21:00"}
                ListElement {hour : "21:30"}
                ListElement {hour : "22:00"}
                ListElement {hour : "22:30"}
                ListElement {hour : "23:00"}
                ListElement {hour : "23:30"}
                ListElement {hour : "0:00"}
            }

        delegate :  Component {
            Item {
                width : timeList.width
                height : timeList.height / 10
                Column {
                    Rectangle {
                        height : 1
                        width : timeList.width
                        color : "black"
                    }
                    Text {
                        text: hour
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked : timeList.currentIndex = index
                }
            }
        }

        highlight:  Component {

            Rectangle {
                width: timeList.width; height: timeList.height /10 ; color: "lightsteelblue" ;radius: 5
            }
        }

        Component.onCompleted : {
            positionViewAtIndex(currentIndex, ListView.Beginning);
        }
    }

}
